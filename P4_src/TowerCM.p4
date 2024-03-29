#include <core.p4>
#include <tna.p4>

/*************************************************************************
 ************* C O N S T A N T S    A N D   T Y P E S  *******************
*************************************************************************/
enum bit<16> ether_type_t {
    TPID       = 0x8100,
    IPV4       = 0x0800
}
enum bit<8>  ip_proto_t {
    ICMP  = 1,
    IGMP  = 2,
    TCP   = 6,
    UDP   = 17
}

type bit<48> mac_addr_t;

/*************************************************************************
 ***********************  H E A D E R S  *********************************
 *************************************************************************/
/*  Define all the headers the program will recognize             */
/*  The actual sets of headers processed by each gress can differ */

/* Standard ethernet header */
header ethernet_h {
    mac_addr_t    dst_addr;
    mac_addr_t    src_addr;
    ether_type_t  ether_type;
}


header ipv4_h {
    bit<4>       version;
    bit<4>       ihl;
    bit<8>       diffserv;
    bit<16>      total_len;
    bit<16>      identification;
    bit<3>       flags;
    bit<13>      frag_offset;
    bit<8>       ttl;
    bit<8>   protocol;
    bit<16>      hdr_checksum;
    bit<32>  src_addr;
    bit<32>  dst_addr;
}


header icmp_h {
    bit<16>  type_code;
    bit<16>  checksum;
}

header igmp_h {
    bit<16>  type_code;
    bit<16>  checksum;
}

header tcp_h {
    bit<16>  src_port;
    bit<16>  dst_port;
    bit<32>  seq_no;
    bit<32>  ack_no;
    bit<4>   data_offset;
    bit<4>   res;
    bit<8>   flags;
    bit<16>  window;
    bit<16>  checksum;
    bit<16>  urgent_ptr;
}

header udp_h {
    bit<16>  src_port;
    bit<16>  dst_port;
    bit<16>  len;
    bit<16>  checksum;
}

/*************************************************************************
 **************  I N G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/
 
    /***********************  H E A D E R S  ************************/

struct my_ingress_headers_t {
    ethernet_h         ethernet;

    ipv4_h             ipv4;
    icmp_h             icmp;
    igmp_h             igmp;
    tcp_h              tcp;
    udp_h              udp;
}

    /******  G L O B A L   I N G R E S S   M E T A D A T A  *********/


struct my_ingress_metadata_t {
    bit<32> ll;
    bit<18> index_2;
    bit<16> index_32;
    bit<16> index_16;
    bit<16> index_8;
    bit<16> index_64;
    bit<32> result_2;
    bit<32> result_32;
    bit<32> result_16;
    bit<32> result_8;
    bit<32> result_64_lo;
    bit<32> result_64_hi;
}

    /***********************  P A R S E R  **************************/

parser IngressParser(packet_in        pkt,
    /* User */
    out my_ingress_headers_t          hdr,
    out my_ingress_metadata_t         meta,
    /* Intrinsic */
    out ingress_intrinsic_metadata_t  ig_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    state start {
        pkt.extract(ig_intr_md);
        pkt.advance(PORT_METADATA_SIZE);
        transition parse_ethernet;
    }

    
    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        /* 
         * The explicit cast allows us to use ternary matching on
         * serializable enum
         */        
        transition select((bit<16>)hdr.ethernet.ether_type) {
            (bit<16>)ether_type_t.IPV4            :  parse_ipv4;
            default :  accept;
        }
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        meta.ll = pkt.lookahead<bit<32>>();
        transition select(hdr.ipv4.protocol) {
            1  : parse_icmp;
            2  : parse_igmp;
            6   : parse_tcp;
            17  : parse_udp;
            default : accept;
    }
    }
    state parse_icmp {
        pkt.extract(hdr.icmp);
        transition accept;
    }
    
    state parse_igmp {
        pkt.extract(hdr.igmp);
        transition accept;
    }
    
    state parse_tcp {
        pkt.extract(hdr.tcp);
        transition accept;
    }
    
    state parse_udp {
        pkt.extract(hdr.udp);
        transition accept;
    }
}
control Ingress(/* User */
    inout my_ingress_headers_t                       hdr,
    inout my_ingress_metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_t               ig_intr_md,
    in    ingress_intrinsic_metadata_from_parser_t   ig_prsr_md,
    inout ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md,
    inout ingress_intrinsic_metadata_for_tm_t        ig_tm_md)
{
    CRCPolynomial<bit<32>>(0x11111111,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32a;
    CRCPolynomial<bit<32>>(0x12222222,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32b;
    CRCPolynomial<bit<32>>(0x13333333,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32c;
    CRCPolynomial<bit<32>>(0x14444444,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32d;
    CRCPolynomial<bit<32>>(0x15555555,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32e;

    Hash<bit<18>>(HashAlgorithm_t.CRC32,crc32a) hash_2;
    Hash<bit<16>>(HashAlgorithm_t.CRC32,crc32b) hash_8;
    Hash<bit<15>>(HashAlgorithm_t.CRC32,crc32c) hash_16;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32d) hash_32;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32d) hash_64;
    
    Register<bit<32>, bit<16>>(0x2000) layer_64bit_lo;
    RegisterAction<bit<32>, bit<16>, bit<32>>(layer_64bit_lo) insert_64bit_lo=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
        
            register_data = register_data + 1;
            result = register_data;
        }
    };

    Register<bit<32>, bit<16>>(0x2000) layer_64bit_hi;
    RegisterAction<bit<32>, bit<16>, bit<32>>(layer_64bit_hi) insert_64bit_hi=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
            if (meta.result_64_lo == 0)
                register_data = register_data |+| 1;
            result = register_data;
        }
    };



    Register<bit<32>, bit<16>>(0x4000) layer_32bit;
    RegisterAction<bit<32>, bit<16>, bit<32>>(layer_32bit) insert_32bit=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
        
            register_data = register_data |+| 1;
            result = register_data;
        }
    };
    Register<bit<16>, bit<16>>(0x8000) layer_16bit;
    RegisterAction<bit<16>, bit<16>, bit<32>>(layer_16bit) insert_16bit=
    {
        void apply(inout bit<16> register_data, out bit<32> result) 
        {
            register_data = register_data |+| 1;
            result = (bit<32>)register_data;
        }
    };
    Register<bit<8>, bit<16>>(0x10000) layer_8bit;
    RegisterAction<bit<8>, bit<16>, bit<32>>(layer_8bit) insert_8bit=
    {
        void apply(inout bit<8> register_data, out bit<32> result) 
        {   
            register_data = register_data |+| 1;
            result = (bit<32>)register_data;
           
        }
    };

    Register<bit<1>, bit<18>>(0x40000) layer_2bit_1;
    RegisterAction<bit<1>, bit<18>, bit<1>>(layer_2bit_1) insert_2bit_1=
    {
        void apply(inout bit<1> register_data, out bit<1> result) 
        {   
            result = register_data;
            register_data = 1;
           
        }
    };

    Register<bit<1>, bit<18>>(0x40000) layer_2bit_2;
    RegisterAction<bit<1>, bit<18>, bit<1>>(layer_2bit_2) insert_2bit_2=
    {
        void apply(inout bit<1> register_data, out bit<1> result) 
        {   
            result = register_data;
            register_data = 1;
           
        }
    };
    Register<bit<1>, bit<18>>(0x40000) layer_2bit_3;
    RegisterAction<bit<1>, bit<18>, bit<1>>(layer_2bit_3) insert_2bit_3=
    {
        void apply(inout bit<1> register_data, out bit<1> result) 
        {   
            result = register_data;
            register_data = 1;
           
        }
    };
    action calindex_layer_64bit()//index
    {
        meta.index_64[12:0] = hash_64.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll});
    }
    table calindex_layer_64bit_t
    {
        actions={calindex_layer_64bit;}
        default_action=calindex_layer_64bit;
    }

    action calindex_layer_2bit()//index
    {
        meta.index_2 = hash_2.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll});
    }
    table calindex_layer_2bit_t
    {
        actions={calindex_layer_2bit;}
        default_action=calindex_layer_2bit;
    }


    action calindex_layer_32bit()//index
    {
        meta.index_32[13:0] = hash_32.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll});
    }
    table calindex_layer_32bit_t
    {
        actions={calindex_layer_32bit;}
        default_action=calindex_layer_32bit;
    }
    action calindex_layer_16bit()//index
    {
        meta.index_16[14:0] = hash_16.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll});
    }
    table calindex_layer_16bit_t
    {
        actions={calindex_layer_16bit;}
        default_action=calindex_layer_16bit;
    }
    action calindex_layer_8bit()//index
    {
        meta.index_8 = hash_8.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll});
    }
    table calindex_layer_8bit_t
    {
        actions={calindex_layer_8bit;}
        default_action=calindex_layer_8bit;
    }
    bit<1> res = 0;
    apply
    {
        calindex_layer_2bit_t.apply();
        calindex_layer_8bit_t.apply();
        calindex_layer_16bit_t.apply();
        calindex_layer_32bit_t.apply();
        calindex_layer_64bit_t.apply();
        meta.result_64_lo = insert_64bit_lo.execute(meta.index_64);
        meta.result_64_hi = insert_64bit_hi.execute(meta.index_64);
        meta.result_32 = insert_32bit.execute(meta.index_32);
        meta.result_16 = insert_16bit.execute(meta.index_16);
        meta.result_8 = insert_8bit.execute(meta.index_8);
        res = insert_2bit_1.execute(meta.index_2);
        meta.result_2 = 1;
        if (res != 0)
        {
            res = insert_2bit_2.execute(meta.index_2);
            meta.result_2 = 2;
            if (res != 0 )
            {
                res = insert_2bit_3.execute(meta.index_2);
                meta.result_2 = 3;
            }
        }
    }
}

control IngressDeparser(packet_out pkt,
    /* User */
    inout my_ingress_headers_t                       hdr,
    in    my_ingress_metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md)
{
    apply {
        pkt.emit(hdr);
    }
}
/*************************************************************************
 ****************  E G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/

    /***********************  H E A D E R S  ************************/

struct my_egress_headers_t {
}

    /********  G L O B A L   E G R E S S   M E T A D A T A  *********/

struct my_egress_metadata_t {
}

    /***********************  P A R S E R  **************************/

parser EgressParser(packet_in        pkt,
    /* User */
    out my_egress_headers_t          hdr,
    out my_egress_metadata_t         meta,
    /* Intrinsic */
    out egress_intrinsic_metadata_t  eg_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    state start {
        pkt.extract(eg_intr_md);
        transition accept;
    }
}

    /***************** M A T C H - A C T I O N  *********************/

control Egress(
    /* User */
    inout my_egress_headers_t                          hdr,
    inout my_egress_metadata_t                         meta,
    /* Intrinsic */    
    in    egress_intrinsic_metadata_t                  eg_intr_md,
    in    egress_intrinsic_metadata_from_parser_t      eg_prsr_md,
    inout egress_intrinsic_metadata_for_deparser_t     eg_dprsr_md,
    inout egress_intrinsic_metadata_for_output_port_t  eg_oport_md)
{
    apply {
    }
}

    /*********************  D E P A R S E R  ************************/

control EgressDeparser(packet_out pkt,
    /* User */
    inout my_egress_headers_t                       hdr,
    in    my_egress_metadata_t                      meta,
    /* Intrinsic */
    in    egress_intrinsic_metadata_for_deparser_t  eg_dprsr_md)
{
    apply {
        pkt.emit(hdr);
    }
}


/************ F I N A L   P A C K A G E ******************************/
Pipeline(
    IngressParser(),
    Ingress(),
    IngressDeparser(),
    EgressParser(),
    Egress(),
    EgressDeparser()
) pipe;

Switch(pipe) main;
