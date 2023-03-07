#include <core.p4>
#include <tna.p4>
#define SWITCH_ID 1
#define HH_thresh 500
#define IL_thresh_offset 4
//hash_## L ##.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll, hdr.INT.SID_## S ##})

#define Tower_32(L,S,P)     \
Register<bit<32>, bit<13>>(8192) switch_## S ##_layer_## L ##_Tower; \
RegisterAction<bit<32>, bit<13>, bit<32>>(switch_## S ##_layer_## L ##_Tower) insert_switch_## S ##_layer_## L ##_Tower = \
{ \
    void apply(inout bit<32> register_data, out bit<32> result) { \
    if (meta.lat_## S ## == 65535 || meta.lat_## S ## > register_data){ \
            result = register_data; \
        register_data = register_data |+| hdr.INT.latency_## S ##; }\
    } \ 
}; \
action insert_switch_## S ##_layer_## L ##_Tower_a(){  \
    meta.lat_## S ## = insert_switch_## S ##_layer_## L ##_Tower.execute(hash_## L #### S ##.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll, hdr.INT.SID_## S ##}));              \
} \
@stage(## P ##) table insert_switch_## S ##_layer_## L ##_Tower_t {           \
    actions = {  \
        insert_switch_## S ##_layer_## L ##_Tower_a; \
    } \
    default_action = insert_switch_## S ##_layer_## L ##_Tower_a; \
} \

#define Tower_16(L,S,P)     \
Register<bit<16>, bit<14>>(16384) switch_## S ##_layer_## L ##_Tower; \
RegisterAction<bit<16>, bit<14>, bit<16>>(switch_## S ##_layer_## L ##_Tower) insert_switch_## S ##_layer_## L ##_Tower = \
{ \
    void apply(inout bit<16> register_data, out bit<16> result) { \
            result = register_data; \
        register_data = register_data |+| meta.INT_latency_## S ##; \
    } \ 
}; \
action insert_switch_## S ##_layer_## L ##_Tower_a(){  \
    meta.lat_## S ##[15:0] = insert_switch_## S ##_layer_## L ##_Tower.execute(hash_## L #### S ##.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll, hdr.INT.SID_## S ##}));              \
} \
@stage(## P ##) table insert_switch_## S ##_layer_## L ##_Tower_t {           \
    actions = {  \
        insert_switch_## S ##_layer_## L ##_Tower_a; \
    } \
    default_action = insert_switch_## S ##_layer_## L ##_Tower_a; \
} \


#define get_lat_hi_bits(S,P) \
action get_lat_hi_4bits_switch_## S ##(bit<4> hi, bit<8> offset) \
{ \
    meta.switch_## S ##_lat_hi = hi; \
    meta.switch_## S ##_lat_offset = offset; \
} \
@stage (## P ##) table get_lat_hi_4bits_switch_## S ##_t {           \
    key = {meta.lat_## S ##:ternary;} \
    actions = {  \
        get_lat_hi_4bits_switch_## S ##; \
    } \
    default_action = get_lat_hi_4bits_switch_## S ##(0,0); \
} \


#define get_INT_lat_hi_bits(S,P) \
action get_INT_lat_hi_4bits_switch_## S ##(bit<4> hi, bit<8> offset) \
{ \
    meta.switch_## S ##_INT_lat_hi = hi; \
    meta.switch_## S ##_INT_lat_offset = offset + meta.freq_offset; \
} \
@stage (## P ##) table get_INT_lat_hi_4bits_switch_## S ##_t {           \
    key = {hdr.INT.latency_## S ##:ternary;} \
    actions = {  \
        get_INT_lat_hi_4bits_switch_## S ##; \
    } \
    default_action = get_INT_lat_hi_4bits_switch_## S ##(0,0); \
} \


#define get_INT_freq_lat_hi_bits(S,P) \
action get_INT_freq_lat_hi_4bits_switch_## S ##(bit<4> hi, bit<8> offset) \
{ \
    meta.switch_## S ##_INT_lat_hi = hi; \
    meta.switch_## S ##_INT_lat_offset = meta.switch_## S ##_INT_lat_offset + offset; \
} \
@stage (## P ##) table get_INT_freq_lat_hi_4bits_switch_## S ##_t {           \
    key = {meta.switch_## S ##_INT_lat_hi:exact; meta.freq_hi:exact;} \
    actions = {  \
        get_INT_freq_lat_hi_4bits_switch_## S ##; \
    } \
    default_action = get_INT_freq_lat_hi_4bits_switch_## S ##(0,0); \
} \
/*************************************************************************
 ************* C O N S T A N T S    A N D   T Y P E S  *******************
*************************************************************************/
enum bit<16> ether_type_t {
    TPID       = 0x8100,
    IPV4       = 0x0800,
    MIRROR     = 0x1111
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

header INT_h
{
    bit<16> SID_1;
    bit<32> latency_1;
    bit<16> SID_2;
    bit<32> latency_2;
    bit<16> SID_3;
    bit<32> latency_3;
    bit<16> SID_4;
    bit<32> latency_4;
    bit<16> SID_5;
    bit<32> latency_5;
}
header bridge_h
{
    bit<32> freq;
    
}
header Mirror_h
{
    bit<48> macdst;
    bit<48> macsrc;
    bit<16> ethtype;
    // bit<32> ipsrc;
    // bit<32> ipdst;
    // bit<32> ll;
    // bit<32> IL_switch_ID;
    bit<8> IL_map;

}



header INT_mirror_h
{
    // bit<32> ipsrc;
    // bit<32> ipdst;
    // bit<32> ll;
    // bit<32> IL_switch_ID;
    bit<8> IL_map;

}
/*************************************************************************
 **************  I N G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/
 
    /***********************  H E A D E R S  ************************/

struct my_ingress_headers_t {
    ethernet_h         ethernet;
    INT_h              INT;
    ipv4_h             ipv4;
    icmp_h             icmp;
    igmp_h             igmp;
    tcp_h              tcp;
    udp_h              udp;
    bridge_h           bridge;
}

    /******  G L O B A L   I N G R E S S   M E T A D A T A  *********/


struct my_ingress_metadata_t {
    bit<32> ll;
    bit<32> ipsrc;
    bit<32> ipdst;
    bit<32> mini_freq;
    bit<32> SID_12;
    bit<32> SID_34;
    bit<32> SID_5_proto;
}

struct pair_t
{
    bit<32> hi;
    bit<32> lo;
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
            (bit<16>)ether_type_t.IPV4            :  parse_INT;
            default :  accept;
        }
    }
    state parse_INT {
        pkt.extract(hdr.INT);
        
        transition parse_ipv4;
    }
    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        meta.mini_freq = 0xffffffff;
        meta.ll = pkt.lookahead<bit<32>>();
        meta.ipsrc = hdr.ipv4.src_addr;
        meta.ipdst = hdr.ipv4.dst_addr;
        meta.SID_5_proto[7:0] = hdr.ipv4.protocol;
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
        hdr.bridge.setValid();
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
    CRCPolynomial<bit<32>>(0x11111111,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_HH;
    CRCPolynomial<bit<32>>(0x12222222,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32b;
    CRCPolynomial<bit<32>>(0x13333333,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32c;
    CRCPolynomial<bit<32>>(0x14444444,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32d;

    Hash<bit<10>>(HashAlgorithm_t.CRC32,crc32_HH) hash_hh;
    Hash<bit<16>>(HashAlgorithm_t.CRC32,crc32b) hash_8;
    Hash<bit<15>>(HashAlgorithm_t.CRC32,crc32c) hash_16;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32d) hash_32;
   
    Register<bit<32>, bit<14>>(0x4000) layer_32bit;
    RegisterAction<bit<32>, bit<14>, bit<32>>(layer_32bit) insert_32bit=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
            if (register_data < meta.mini_freq)
            {
                result = register_data;
                register_data = register_data |+| 1;
            }
        }
    };
    Register<bit<16>, bit<15>>(0x8000) layer_16bit;
    RegisterAction<bit<16>, bit<15>, bit<32>>(layer_16bit) insert_16bit=
    {
        void apply(inout bit<16> register_data, out bit<32> result) 
        {
            if (register_data != 65535 && register_data < (bit<16>)meta.mini_freq)
            result = (bit<32>)register_data;

            if (register_data < (bit<16>)meta.mini_freq)
            register_data = register_data |+| 1;
        }
    };
    Register<bit<8>, bit<16>>(0x10000) layer_8bit;
    RegisterAction<bit<8>, bit<16>, bit<32>>(layer_8bit) insert_8bit=
    {
        void apply(inout bit<8> register_data, out bit<32> result) 
        {   
            if (register_data != 255)
            result = (bit<32>)register_data;

            register_data = register_data |+| 1;
           
        }
    };

    Register<pair_t, bit<10>>(1024) hash_table_HH_1;
    Register<pair_t, bit<10>>(1024) hash_table_HH_2;
    Register<pair_t, bit<10>>(1024) hash_table_HH_3;
    RegisterAction<pair_t, bit<10>, bit<32>>(hash_table_HH_1) insert_HH_1=
    {
        void apply(inout pair_t register_data, out bit<32> result) 
        {   
            if (register_data.hi == 0)
            {
                register_data.hi = meta.ipsrc;
                register_data.lo = meta.SID_12;
            }
           
        }
    };
    RegisterAction<pair_t, bit<10>, bit<32>>(hash_table_HH_2) insert_HH_2=
    {
        void apply(inout pair_t register_data, out bit<32> result) 
        {   
            if (register_data.hi == 0)
            {
                register_data.hi = meta.ipdst;
                register_data.lo = meta.SID_34;
            }
           
        }
    };
    RegisterAction<pair_t, bit<10>, bit<32>>(hash_table_HH_3) insert_HH_3=
    {
        void apply(inout pair_t register_data, out bit<32> result) 
        {   
            if (register_data.hi == 0)
            {
                register_data.hi = meta.ll;
                register_data.lo = meta.SID_5_proto;
            }
           
        }
    };
    bit<32> delta;
    action cal_delta()
    {
        delta = meta.mini_freq |-| HH_thresh;
        hdr.bridge.freq = meta.mini_freq;
    }
    @stage(7) table cal_delta_t
    {
        actions={cal_delta;}
        default_action=cal_delta;
    }
    
    apply
    {
        if (hdr.tcp.isValid())
        {   hdr.INT.SID_5 = SWITCH_ID;
            meta.SID_12 = hdr.INT.SID_1 ++ hdr.INT.SID_2;
            meta.SID_34 = hdr.INT.SID_3 ++ hdr.INT.SID_4;
            meta.SID_5_proto[31:16] = SWITCH_ID;
            meta.mini_freq = insert_8bit.execute(hash_8.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
            meta.mini_freq = insert_16bit.execute(hash_16.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
            meta.mini_freq = insert_32bit.execute(hash_32.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
            
            
            cal_delta_t.apply();
            if (delta != 0)
            {
                insert_HH_1.execute(hash_hh.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
                insert_HH_2.execute(hash_hh.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
                insert_HH_3.execute(hash_hh.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll}));
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
    ethernet_h         ethernet;
    INT_mirror_h       INT_mirror;
    INT_h              INT;
    ipv4_h             ipv4;
    icmp_h             icmp;
    igmp_h             igmp;
    tcp_h              tcp;
    udp_h              udp;
    bridge_h           bridge;
}

    /********  G L O B A L   E G R E S S   M E T A D A T A  *********/

struct my_egress_metadata_t {
    bit<16> INT_latency_1;
    bit<16> INT_latency_2;
    bit<16> INT_latency_3;
    bit<16> INT_latency_4;
    bit<16> INT_latency_5;
    Mirror_h mirror_ins;
    bit<32> ipsrc;
    bit<32> ipdst;
    bit<32> ll;
    bit<32> mini_freq;
    bit<32> lat_1;
    bit<32> lat_2;
    bit<32> lat_3;
    bit<32> lat_4;
    bit<32> lat_5;
    bit<4> switch_1_lat_hi;
    bit<4> switch_2_lat_hi;
    bit<4> switch_3_lat_hi;
    bit<4> switch_4_lat_hi;
    bit<4> switch_5_lat_hi;
    bit<8> switch_1_lat_offset;
    bit<8> switch_2_lat_offset;
    bit<8> switch_3_lat_offset;
    bit<8> switch_4_lat_offset;
    bit<8> switch_5_lat_offset;
    bit<4> freq_hi;
    bit<8> freq_offset;
    bit<4> switch_1_INT_lat_hi;
    bit<4> switch_2_INT_lat_hi;
    bit<4> switch_3_INT_lat_hi;
    bit<4> switch_4_INT_lat_hi;
    bit<4> switch_5_INT_lat_hi;
    bit<8> switch_1_INT_lat_offset;
    bit<8> switch_2_INT_lat_offset;
    bit<8> switch_3_INT_lat_offset;
    bit<8> switch_4_INT_lat_offset;
    bit<8> switch_5_INT_lat_offset;
    bit<1> inf_s1;
    bit<1> inf_s2;
    bit<1> inf_s3;
    bit<1> inf_s4;
    bit<1> inf_s5;
    bit<8> IL_map;
    bit<32> IL_switch_ID;
    MirrorId_t session_id;
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
        transition parse_ethernet;
    }

    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        /* 
         * The explicit cast allows us to use ternary matching on
         * serializable enum
         */        
        transition select((bit<16>)hdr.ethernet.ether_type) {
            (bit<16>)ether_type_t.IPV4            :  parse_INT;
            (bit<16>)ether_type_t.MIRROR            :  parse_INT_mirror;
            default :  accept;
        }
    }
    state parse_INT_mirror {
        pkt.extract(hdr.INT_mirror);
        // meta.ipsrc = hdr.INT_mirror.ipsrc;
        // meta.ipdst = hdr.INT_mirror.ipdst;
        // meta.ll = hdr.INT_mirror.ll;
        // meta.IL_switch_ID = hdr.INT_mirror.IL_switch_ID;
        meta.IL_map = hdr.INT_mirror.IL_map;
        transition parse_advance;
    }
    state parse_advance
    {
        pkt.advance(112);
        transition parse_INT;
    }
    state parse_INT {
        pkt.extract(hdr.INT);
        //hdr.INT.latency_5 = (bit<32>)eg_intr_md.deq_timedelta;
        meta.INT_latency_1 = hdr.INT.latency_1[15:0];
        meta.INT_latency_2 = hdr.INT.latency_2[15:0];
        meta.INT_latency_3 = hdr.INT.latency_3[15:0];
        meta.INT_latency_4 = hdr.INT.latency_4[15:0];
        meta.INT_latency_5 = hdr.INT.latency_5[15:0];
        transition parse_ipv4;
    }
    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        meta.ll = pkt.lookahead<bit<32>>();
        meta.ipsrc = hdr.ipv4.src_addr;
        meta.ipdst = hdr.ipv4.dst_addr;
        meta.IL_switch_ID[7:0] = hdr.ipv4.protocol;
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
        transition parse_bridge;
    }
    state parse_bridge {
        pkt.extract(hdr.bridge);
        meta.mini_freq = hdr.bridge.freq;
        transition accept;
    }
    state parse_udp {
        pkt.extract(hdr.udp);
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
    CRCPolynomial<bit<32>>(0x22222222,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_1;
    CRCPolynomial<bit<32>>(0x32222222,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_2;
    CRCPolynomial<bit<32>>(0x43333333,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_3;
    CRCPolynomial<bit<32>>(0x44444444,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_IL;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32_1) hash_11;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32_2) hash_21;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32_1) hash_12;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32_2) hash_22;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32_1) hash_13;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32_2) hash_23;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32_1) hash_14;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32_2) hash_24;
    Hash<bit<14>>(HashAlgorithm_t.CRC32,crc32_1) hash_15;
    Hash<bit<13>>(HashAlgorithm_t.CRC32,crc32_2) hash_25;
    Hash<bit<10>>(HashAlgorithm_t.CRC32,crc32_IL) hash_IL;
    Tower_16(1,1,0)
    Tower_32(2,1,1)
    Tower_16(1,2,0)
    Tower_32(2,2,1)
    Tower_16(1,3,0)
    Tower_32(2,3,1)
    Tower_16(1,4,2)
    Tower_32(2,4,3)
    Tower_16(1,5,2)
    Tower_32(2,5,3)
    get_lat_hi_bits(1,4)
    get_lat_hi_bits(2,4)
    get_lat_hi_bits(3,4)
    get_lat_hi_bits(4,4)
    get_lat_hi_bits(5,4)
    get_INT_lat_hi_bits(1,5)
    get_INT_lat_hi_bits(2,5)
    get_INT_lat_hi_bits(3,5)
    get_INT_lat_hi_bits(4,5)
    get_INT_lat_hi_bits(5,5)
    get_INT_freq_lat_hi_bits(1,6)
    get_INT_freq_lat_hi_bits(2,6)
    get_INT_freq_lat_hi_bits(3,6)
    get_INT_freq_lat_hi_bits(4,6)
    get_INT_freq_lat_hi_bits(5,6)
    action get_freq_hi(bit<4> hi, bit<8> offset)
    {
        meta.freq_hi = hi; 
        meta.freq_offset = offset; 
    } 
    @stage (4) table get_freq_hi_t {           
        key = {meta.mini_freq:ternary;} 
        actions = {  
            get_freq_hi; 
        } 
        default_action = get_freq_hi(0,0); 
    } 
    action get_delta()
    {
        meta.switch_1_INT_lat_hi = meta.switch_1_INT_lat_hi |-| meta.switch_1_lat_hi;
       meta.switch_2_INT_lat_hi = meta.switch_2_INT_lat_hi |-| meta.switch_2_lat_hi;
        meta.switch_3_INT_lat_hi = meta.switch_3_INT_lat_hi |-| meta.switch_3_lat_hi;
        meta.switch_4_INT_lat_hi = meta.switch_4_INT_lat_hi |-| meta.switch_4_lat_hi;
         meta.switch_5_INT_lat_hi = meta.switch_5_INT_lat_hi |-| meta.switch_5_lat_hi;
    }
    @stage (7) table get_delta_t
    {
        actions = {get_delta;}
        default_action = get_delta;
    }


    action get_delta_2()
    {

        meta.switch_1_INT_lat_offset = meta.switch_1_INT_lat_offset |-| meta.switch_1_lat_offset;
        meta.switch_2_INT_lat_offset = meta.switch_2_INT_lat_offset |-| meta.switch_2_lat_offset;
        meta.switch_3_INT_lat_offset = meta.switch_3_INT_lat_offset |-| meta.switch_3_lat_offset;
        meta.switch_4_INT_lat_offset = meta.switch_4_INT_lat_offset |-| meta.switch_4_lat_offset;
        meta.switch_5_INT_lat_offset = meta.switch_5_INT_lat_offset |-| meta.switch_5_lat_offset;
    }
    @stage (7) table get_delta_2_t
    {
        actions = {get_delta_2;}
        default_action = get_delta_2;
    }

    action get_IL_1(bit<8> IL_map)
    {
        meta.IL_map = IL_map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_1;
    }
    action get_IL_2(bit<8> IL_map)
    {
        meta.IL_map = IL_map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_2;
    }
    action get_IL_3(bit<8> IL_map)
    {
        meta.IL_map = IL_map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_3;
    }
    action get_IL_4(bit<8> IL_map)
    {
        meta.IL_map = IL_map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_4;
    }
    action get_IL_5(bit<8> IL_map)
    {
        meta.IL_map = IL_map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_5;
    }
    @stage (9) table get_IL_t
    {
        key = {meta.inf_s1:exact; meta.inf_s2:exact; meta.inf_s3:exact; meta.inf_s4:exact; meta.inf_s5:exact;}
        actions = {get_IL_1;get_IL_2; get_IL_3; get_IL_4; get_IL_5;}
        default_action = get_IL_1(0);
    }

    Register<pair_t, bit<10>>(1024) hash_table_IL_1;
    Register<pair_t, bit<10>>(1024) hash_table_IL_2;
    RegisterAction<pair_t, bit<10>, bit<32>>(hash_table_IL_1) insert_IL_1=
    {
        void apply(inout pair_t register_data, out bit<32> result) 
        {   
            if (register_data.hi == 0)
            {
                register_data.hi = meta.ipsrc;
                register_data.lo = meta.ipdst;
            }
           
        }
    };
    RegisterAction<pair_t, bit<10>, bit<32>>(hash_table_IL_2) insert_IL_2=
    {
        void apply(inout pair_t register_data, out bit<32> result) 
        {   
            if (register_data.hi == 0)
            {
                register_data.hi = meta.ll;
                register_data.lo = meta.IL_switch_ID;
            }
           
        }
    };


    action mirror_set()
    {
        meta.mirror_ins.ethtype = 0x1111;
        /*meta.mirror_ins.ipsrc = meta.ipsrc;
        meta.mirror_ins.ipdst = meta.ipdst;
        meta.mirror_ins.IL_switch_ID = meta.IL_switch_ID;
        meta.mirror_ins.ll = meta.ll;*/
        meta.mirror_ins.IL_map = meta.IL_map;
        meta.session_id = 1;
        eg_dprsr_md.mirror_type = 1;
    }
    @stage (11) table mirror_set_t
    {
        actions = {mirror_set;}
        default_action = mirror_set;
    }


    action get_IL_from_map_1(bit<8> map)
    {
        meta.IL_map = map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_1;
    }
    action get_IL_from_map_2(bit<8> map)
    {
        meta.IL_map = map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_2;
    }
    action get_IL_from_map_3(bit<8> map)
    {
        meta.IL_map = map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_3;
    }
    action get_IL_from_map_4(bit<8> map)
    {
        meta.IL_map = map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_4;
    }
    action get_IL_from_map_5(bit<8> map)
    {
        meta.IL_map = map;
        meta.IL_switch_ID[31:16] = hdr.INT.SID_5;
    }
    @stage (8) table get_IL_from_map_t
    {
        key = {meta.IL_map:exact;}
        actions = {get_IL_from_map_1;get_IL_from_map_2;get_IL_from_map_3;get_IL_from_map_4;get_IL_from_map_5;}
        default_action = get_IL_from_map_1(0);
    }
    apply {
        if (hdr.tcp.isValid())
        {
            insert_switch_1_layer_1_Tower_t.apply();
            insert_switch_1_layer_2_Tower_t.apply();
            insert_switch_2_layer_1_Tower_t.apply();
            insert_switch_2_layer_2_Tower_t.apply();
            insert_switch_3_layer_1_Tower_t.apply();
            insert_switch_3_layer_2_Tower_t.apply();
            insert_switch_4_layer_1_Tower_t.apply();
            insert_switch_4_layer_2_Tower_t.apply();
            insert_switch_5_layer_1_Tower_t.apply();
            insert_switch_5_layer_2_Tower_t.apply();
            get_lat_hi_4bits_switch_1_t.apply();
            get_lat_hi_4bits_switch_2_t.apply();
            get_lat_hi_4bits_switch_3_t.apply();
            get_lat_hi_4bits_switch_4_t.apply();
            get_lat_hi_4bits_switch_5_t.apply();
            get_freq_hi_t.apply();
            get_INT_lat_hi_4bits_switch_1_t.apply();
            get_INT_lat_hi_4bits_switch_2_t.apply();
            get_INT_lat_hi_4bits_switch_3_t.apply();
            get_INT_lat_hi_4bits_switch_4_t.apply();
            get_INT_lat_hi_4bits_switch_5_t.apply();
            get_INT_freq_lat_hi_4bits_switch_1_t.apply();
            get_INT_freq_lat_hi_4bits_switch_2_t.apply();
            get_INT_freq_lat_hi_4bits_switch_3_t.apply();
            get_INT_freq_lat_hi_4bits_switch_4_t.apply();
            get_INT_freq_lat_hi_4bits_switch_5_t.apply();
            get_delta_t.apply();
            get_delta_2_t.apply();
            if ((meta.switch_1_INT_lat_offset == 0 && meta.switch_1_INT_lat_hi != 0 ) || meta.switch_1_INT_lat_offset != 0)
            {
                meta.inf_s1 = 1;
            }
            if ((meta.switch_2_INT_lat_offset == 0 && meta.switch_2_INT_lat_hi != 0 ) || meta.switch_2_INT_lat_offset != 0)
            {
                meta.inf_s2 = 1;
            }
            if ((meta.switch_3_INT_lat_offset == 0 && meta.switch_3_INT_lat_hi != 0 ) || meta.switch_3_INT_lat_offset != 0)
            {
                meta.inf_s3 = 1;
            }
            if ((meta.switch_4_INT_lat_offset == 0 && meta.switch_4_INT_lat_hi != 0 ) || meta.switch_4_INT_lat_offset != 0)
            {
                meta.inf_s4 = 1;
            }
            if ((meta.switch_5_INT_lat_offset == 0 && meta.switch_5_INT_lat_hi != 0 ) || meta.switch_5_INT_lat_offset != 0)
            {
                meta.inf_s5 = 1;
            }
            get_IL_t.apply();
        }
        else if (hdr.INT_mirror.isValid())
        {
            get_IL_from_map_t.apply();
            eg_dprsr_md.drop_ctl = 1;
        }
        if (meta.IL_switch_ID != 0)
        {
            insert_IL_1.execute(hash_IL.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll, meta.IL_switch_ID}));
            insert_IL_2.execute(hash_IL.get({hdr.ipv4.src_addr,hdr.ipv4.dst_addr,hdr.ipv4.protocol, meta.ll, meta.IL_switch_ID}));
        }
        if (meta.IL_map != 0 )
        {
            mirror_set_t.apply();
        }
        hdr.bridge.setInvalid();
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
    Mirror() mirror;
    apply {
        if (eg_dprsr_md.mirror_type == 1)
        mirror.emit<Mirror_h>(meta.session_id, meta.mirror_ins);
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
