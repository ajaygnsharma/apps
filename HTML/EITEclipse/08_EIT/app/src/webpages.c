#include "micronet.h"
#include "np_includes.h"
/* Added "upgrade" - post_software to the functions
   See the conditional define _POST_FILES
   4.9.2008 - M.J.Butcher                                */

extern void post_software(PSOCKET_INFO socket_ptr) cmx_reentrant;

/*-----------------------------------------------------------------------
Function: 	webpages_init(void)
Description:Initialize the Webpages
Input:		void
Output:		void
------------------------------------------------------------------------*/ 
void webpages_init(void)
{
  mn_vf_set_entry((byte *)"index.htm", INDEX_SIZE, index_htm,\
    VF_PTYPE_STATIC);                                             /* 01 */
  mn_vf_set_entry((byte *)"eit_style.css", EIT_STYLE_SIZE, eit_style_css,
    VF_PTYPE_STATIC);
  mn_vf_set_entry((byte *)"logo.gif",LOGO_SIZE,\
    logo_gif,VF_PTYPE_STATIC);                                    /* 06 */
  mn_vf_set_entry((byte *)"iport.class",IPORT_SIZE,\
    iport_class,VF_PTYPE_STATIC);                                 /* 08 */
  mn_vf_set_entry((byte *)"terminal.htm",TERMINAL_SIZE,\
    terminal_htm,VF_PTYPE_STATIC);                                /* 09 */
  mn_vf_set_entry((byte *)"overview.htm",OVERVIEW_SIZE, overview_htm,\
    VF_PTYPE_STATIC);                                             /* 10 */
  mn_vf_set_entry((byte *)"admin.htm",ADMIN_SIZE,\
    admin_htm,VF_PTYPE_STATIC);                                   /* 11 */
  mn_vf_set_entry((byte *)"network.htm",NETWORK_SIZE,\
    network_htm,VF_PTYPE_STATIC);                                 /* 11 */
  mn_vf_set_entry((byte *)"serial.htm",SERIAL_SIZE,\
    serial_htm,VF_PTYPE_STATIC);                                  /* 13 */
  mn_vf_set_entry((byte *)"bridge.htm",BRIDGE_SIZE,\
    bridge_htm,VF_PTYPE_STATIC);                                  /* 13 */
  mn_vf_set_entry((byte *)"packing.htm",PACKING_SIZE,\
    packing_htm,VF_PTYPE_STATIC);                                  /* 13 */
  mn_vf_set_entry((byte *)"multihost.htm",MULTIHOST_SIZE,\
    multihost_htm,VF_PTYPE_STATIC);                                  /* 13 */
  mn_vf_set_entry((byte *)"s_admin.htm",S_ADMIN_SIZE,\
    s_admin_htm,VF_PTYPE_STATIC);                                   /* 11 */
  mn_vf_set_entry((byte *)"security.htm",SECURITY_SIZE,\
    security_htm,VF_PTYPE_STATIC);                                /* 15 */
  mn_vf_set_entry((byte *)"device_setup.htm",DEVICE_SETUP_SIZE,\
    device_setup_htm,VF_PTYPE_STATIC);                            /* 16 */
  mn_vf_set_entry((byte *)"device_query.htm",DEVICE_QUERY_SIZE,\
    device_query_htm,VF_PTYPE_STATIC);                            /* 17 */
  mn_vf_set_entry((byte *)"system_admin.htm",SYSTEM_ADMIN_SIZE,\
    system_admin_htm,VF_PTYPE_STATIC);                                   /* 11 */
  mn_vf_set_entry((byte *)"reboot.htm",REBOOT_SIZE,\
    reboot_htm,VF_PTYPE_STATIC);                                  /* 19 */
  mn_vf_set_entry((byte *)"defaults.htm",DEFAULTS_SIZE,\
    defaults_htm,VF_PTYPE_STATIC);                                /* 20 */
  mn_vf_set_entry((byte *)"upgrade.htm",UPGRADE_SIZE,\
    upgrade_htm,VF_PTYPE_STATIC);                                 /* 21 */
  mn_vf_set_entry((byte *)"diagnostics.htm",DIAGNOSTICS_SIZE,\
    diagnostics_htm,VF_PTYPE_STATIC);                             /* 22 */
  mn_vf_set_entry((byte *)"iopins.htm",IOPINS_SIZE,\
    iopins_htm,VF_PTYPE_STATIC);                                  /* 23 */
  mn_vf_set_entry((byte *)"change_login.htm",CHANGE_LOGIN_SIZE,\
    change_login_htm,VF_PTYPE_STATIC);                            /* 24 */
  mn_vf_set_entry((byte *)"change_admin.htm",CHANGE_ADMIN_SIZE,\
    change_admin_htm,VF_PTYPE_STATIC);                            /* 25 */
  mn_vf_set_entry((byte *)"change_serial.htm",CHANGE_SERIAL_SIZE,\
    change_serial_htm,VF_PTYPE_STATIC);                           /* 26 */
  mn_vf_set_entry((byte *)"setdeviceparam.htm",SETDEVICEPARAM_SIZE,\
    setdeviceparam_htm,VF_PTYPE_STATIC);                          /* 27 */
  mn_vf_set_entry((byte *)"read_device.htm",READ_DEVICE_SIZE,\
    read_device_htm,VF_PTYPE_STATIC);                             /* 28 */
  mn_vf_set_entry((byte *)"alarm1.htm",ALARM1_SIZE,\
    alarm1_htm,VF_PTYPE_STATIC);                                  /* 29 */
  mn_vf_set_entry((byte *)"alarm2.htm",ALARM2_SIZE,\
    alarm2_htm,VF_PTYPE_STATIC);                                  /* 30 */
  mn_vf_set_entry((byte *)"alarm3.htm",ALARM3_SIZE,\
    alarm3_htm,VF_PTYPE_STATIC);                                  /* 31 */
  mn_vf_set_entry((byte *)"alarm4.htm",ALARM4_SIZE,\
    alarm4_htm,VF_PTYPE_STATIC);                                  /* 32 */
  mn_vf_set_entry((byte *)"alarm5.htm",ALARM5_SIZE,\
    alarm5_htm,VF_PTYPE_STATIC);                                  /* 33 */
  mn_vf_set_entry((byte *)"snmp_email.htm",SNMP_EMAIL_SIZE,\
    snmp_email_htm,VF_PTYPE_STATIC);                              /* 32 */
  mn_vf_set_entry((byte *)"grey.jpg",GREY_SIZE,\
    grey_jpg,VF_PTYPE_STATIC);                                     /* 45 */                 
  mn_vf_set_entry((byte *)"green.jpg",GREEN_SIZE,\
    green_jpg,VF_PTYPE_STATIC);                                   /* 46 */                 
  mn_vf_set_entry((byte *)"white.jpg",WHITE_SIZE,\
    white_jpg,VF_PTYPE_STATIC);                                   /* 47 */
  mn_vf_set_entry((byte *)"host_active.htm",HOST_ACTIVE_SIZE,\
    host_active_htm,VF_PTYPE_STATIC);                              /* 32 */
  mn_vf_set_entry((byte *)"host_down.htm",HOST_DOWN_SIZE,\
    host_down_htm,VF_PTYPE_STATIC);                              /* 32 */
  mn_vf_set_entry((byte *)"t_wait.htm",T_WAIT_SIZE,\
    t_wait_htm,VF_PTYPE_STATIC);                       /* 32 */
  
  
  /* add post functions to be used with forms */
  mn_pf_set_entry((byte *)"main", main_func);	  /* Main Page Function */ 
  mn_pf_set_entry((byte *)"admin", post_admin);	  /* Main Page Function */ 
  mn_pf_set_entry((byte *)"network",network_func);/*Set Network Settings*/
  mn_pf_set_entry((byte *)"network_pass",post_network); /*Set Network Settings*/
  mn_pf_set_entry((byte *)"serial",post_serial);  /* Serial Config      */
  mn_pf_set_entry((byte *)"bridge",post_bridge);  /* Serial Config      */
  mn_pf_set_entry((byte *)"packing",post_packing);/* Serial Config  */
  mn_pf_set_entry((byte *)"multihost",post_multihost);/* Serial Config  */
  mn_pf_set_entry((byte *)"smtpsnmp",post_smtpsnmp);/* Email & SNMP Cfg */
  mn_pf_set_entry((byte *)"alarm1",post_alarm1);    /* Alarm1       Cfg */
  mn_pf_set_entry((byte *)"alarm2",post_alarm2);    /* Alarm2       Cfg */
  mn_pf_set_entry((byte *)"alarm3",post_alarm3);    /* Alarm3       Cfg */
  mn_pf_set_entry((byte *)"alarm4",post_alarm4);    /* Alarm4       Cfg */
  mn_pf_set_entry((byte *)"alarm5",post_alarm5);    /* Alarm5       Cfg */
  mn_pf_set_entry((byte *)"s_admin", post_security_admin);	  /* Main Page Function */ 
  mn_pf_set_entry((byte *)"security",post_security); /* Security Config */
  mn_pf_set_entry((byte *)"clogin",post_change_login); /* Change Login  */
  mn_pf_set_entry((byte *)"cadmin",post_change_admin); /* Change Admin  */
  mn_pf_set_entry((byte *)"cserial",post_change_serial);/* Change Serial*/
  mn_pf_set_entry((byte *)"setdeviceparam",post_setdeviceparam);/* dev p*/
  mn_pf_set_entry((byte *)"updatedevparam",post_updatedevparam);/* dev p*/
  mn_pf_set_entry((byte *)"check",post_check);                  /* dev p*/
  mn_pf_set_entry((byte *)"system_admin", post_system_admin);	  /* Main Page Function */
  mn_pf_set_entry((byte *)"rdef",post_rdef);          /* Defatuls       */
  mn_pf_set_entry((byte *)"reboot",post_reboot);      /* Reboot Iserver */
  mn_pf_set_entry((byte *)"gpio",post_gpio);         /* Set Serial type */
  mn_pf_set_entry((byte *)"command",post_command);   /* Send Command    */
  mn_pf_set_entry((byte *)"set_timeout", set_timeout_func); /* read_dev */
  mn_pf_set_entry((byte *)"read_dev", post_read_dev);       /* read_dev */
  mn_pf_set_entry((byte *)"rst_cnt", post_rst_cnt);         /* reset cnt*/
  mn_pf_set_entry((byte *)"load", post_load);         /* reset cnt*/
  mn_pf_set_entry((byte *)"chk_login_passwd", post_chk_login_passwd);    /* Check Login Password */
  
#ifdef _POST_FILES
  mn_pf_set_entry((byte *)"upgrade", post_software);    /* post data - software upload */
#endif
  
  
  
  
  
  
  /* add any get functions (server-side-includes) here */
  mn_gf_set_entry((byte *)"pswd_msg",ssi_pswd_msg);               /* 01 */

                                  /*---------- overview.htm ------------*/
  mn_gf_set_entry((byte *)"model",model_func);                    /* 01 */
  mn_gf_set_entry((byte *)"version",version_func);/* 02      FW Version */
  mn_gf_set_entry((byte *)"mac", mac_func);       /* 03      MAC Addr   */
  mn_gf_set_entry((byte *)"port", port_func);     /* 04   Ethernet Port */
  mn_gf_set_entry((byte *)"dhcp", dhcp_func);     /* 05   DHCP Flag     */
  mn_gf_set_entry((byte *)"snmp", snmp_func);     /* 06   SNMP Flag     */
  mn_gf_set_entry((byte *)"modbus", modbus_func); /* 07   MODBUS Flag   */
  mn_gf_set_entry((byte *)"ip", ip_func);         /* 08   IP Address    */
  mn_gf_set_entry((byte *)"submask",submask_func);/* 09   Subnet Mask   */
  mn_gf_set_entry((byte *)"gw_ip", gw_ip_func);   /* 10   Gateway IP    */
  mn_gf_set_entry((byte *)"dns", ssi_dns);        /* 10   Gateway IP    */
  mn_gf_set_entry((byte *)"hname", hname_func);   /* 11   Hostname      */
  mn_gf_set_entry((byte *)"serial", serial_func); /* 12   Serial Setting*/

                                  /*----------- network.htm ------------*/
  mn_gf_set_entry((byte *)"dhcpflag", dhcpflag_func);   /* 13 DHCP Flag */
  mn_gf_set_entry((byte *)"protocol", ssi_protocol);              /* 14 */
  mn_gf_set_entry((byte *)"autonegotiate", autonegotiate_func); /* 15   */
  mn_gf_set_entry((byte *)"s10", s10_func);             /* 15 10 Mbps   */
  mn_gf_set_entry((byte *)"s100", s100_func);           /* 16 100 Mbps  */
  mn_gf_set_entry((byte *)"fduplex", fduplex_func);     /* 17 Ful Duplex*/
  mn_gf_set_entry((byte *)"hduplex", hduplex_func);     /* 18 Haf Duplex*/
                                   /*----------- serial.htm ------------*/
  mn_gf_set_entry((byte *)"baud", baud_func);           /* 19 Baudrate  */
  mn_gf_set_entry((byte *)"databits", databits_func);   /* 20 Databits  */
  mn_gf_set_entry((byte *)"parity", parity_func);       /* 21 Parity    */
  mn_gf_set_entry((byte *)"stopbit", stopbit_func);     /* 22 Stop bit  */
  mn_gf_set_entry((byte *)"flowcontrol", flowcontrol_func); /* 23 FlCtrl*/
  mn_gf_set_entry((byte *)"no_of_conns", no_of_conns_func);/*24 no:ofCon*/
  mn_gf_set_entry((byte *)"lp", lp_func);               /* 25 Local Port*/
  mn_gf_set_entry((byte *)"end_char", end_char_func);   /* 28 End Char  */
  mn_gf_set_entry((byte *)"buff_time", buff_time_func); /* 29 Buff Time */
  mn_gf_set_entry((byte *)"itimeout",itimeout_func);/*30 Inactivity TOut*/
  mn_gf_set_entry((byte *)"fwd_endchar", fwd_endchar_func);/*32 Fwd EndC*/
  mn_gf_set_entry((byte *)"pkt_len", ssi_pkt_len);        /* 82 Fwd EndC*/
  mn_gf_set_entry((byte *)"connect_on", ssi_connect_on);  /* 83 Fwd EndC*/
  mn_gf_set_entry((byte *)"disconnect_by", ssi_disconnect_by); /* */
  mn_gf_set_entry((byte *)"connection_type", ssi_connection_type);   /* */
  mn_gf_set_entry((byte *)"retry_counter", ssi_retry_counter);    /* */
  mn_gf_set_entry((byte *)"retry_timeout", ssi_retry_timeout);    /* */
  mn_gf_set_entry((byte *)"multi_ip_addr", ssi_mipaddr);
  mn_gf_set_entry((byte *)"multi_port", ssi_multi_port);
  mn_gf_set_entry((byte *)"WebPort", WebPort_func);

                                   /*----------- security.htm ----------*/
  mn_gf_set_entry((byte *)"spca", ssi_spca);/* 33 Serial P Console Flag */
  mn_gf_set_entry((byte *)"wsa", ssi_wsa);      /* 34 Webserver flag    */
  mn_gf_set_entry((byte *)"ica", ssi_ica);      /* 35 ICA flag          */
  mn_gf_set_entry((byte *)"ipx", ssi_ipx);      /* 36 IP Exclusivity         */
  mn_gf_set_entry((byte *)"lpass", ssi_login);  /* 36 Login Password    */
  mn_gf_set_entry((byte *)"login_msg",ssi_login_msg);               /* 01 */

  mn_gf_set_entry((byte *)"apass", ssi_admin);  /* 37 Admin Password    */
  mn_gf_set_entry((byte *)"admin_msg",ssi_admin_msg);               /* 01 */

  mn_gf_set_entry((byte *)"spass", ssi_serial); /* 38 Serial Password   */
  mn_gf_set_entry((byte *)"serial_msg",ssi_serial_msg);               /* 01 */

  mn_gf_set_entry((byte *)"ip1", ssi_ip1);                        /* 39 */
  mn_gf_set_entry((byte *)"ip2", ssi_ip2);                        /* 39 */
  mn_gf_set_entry((byte *)"ip3", ssi_ip3);                        /* 39 */
  mn_gf_set_entry((byte *)"ip4", ssi_ip4);                        /* 39 */
  mn_gf_set_entry((byte *)"ip5", ssi_ip5);                        /* 39 */
  mn_gf_set_entry((byte *)"ip6", ssi_ip6);                        /* 39 */
  mn_gf_set_entry((byte *)"ip7", ssi_ip7);                        /* 39 */
  mn_gf_set_entry((byte *)"ip8", ssi_ip8);                        /* 39 */
    
  //mn_gf_set_entry((byte *)"mask1", ssi_mask1);                    /* 40 */

                                   /*----------- management.htm --------*/
  mn_gf_set_entry((byte *)"smtp", ssi_smtp);            /* 38 SMTP Flag */
  mn_gf_set_entry((byte *)"smtpip", ssi_smtpip);/* 39 SMTP Server IP    */
  mn_gf_set_entry((byte *)"from", ssi_from);   /* 40 Senders Email Addr */
  mn_gf_set_entry((byte *)"sub", ssi_sub);     /* 41 Subject of Email   */
  
  mn_gf_set_entry((byte *)"snmp_en", ssi_snmp_en);/* 42 SNMP Enabled?   */
  mn_gf_set_entry((byte *)"trap_en", ssi_trap_en);/* 43 TRAP Enabled?   */
  mn_gf_set_entry((byte *)"trap_ip", ssi_trap_ip);/* 44 TRAP IP Enabled?*/
  mn_gf_set_entry((byte *)"contact", ssi_contact);/* 45 Contact Info    */
  mn_gf_set_entry((byte *)"location",ssi_location);/*46 Location Info   */
  mn_gf_set_entry((byte *)"community",ssi_community);/*46 Community String */
  

  /* Alarm 1 */
  //mn_gf_set_entry((byte *)"alarm1_en",ssi_alarm1_en);/*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a1_e_chk",ssi_a1_e_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a1_t_chk",ssi_a1_t_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a1_c1",ssi_a1_c1);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_c2",ssi_a1_c2);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_sspd",ssi_a1_sspd);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_ic_evt",ssi_a1_ic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_sic_evt",ssi_a1_sic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_ri",ssi_a1_ri);      /*60 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a1_to",ssi_a1_to);      /*61 Alarm1 Recipient*/
  mn_gf_set_entry((byte *)"a1_msg",ssi_a1_msg);    /*62 Alarm1 Message  */
  
  /* Alarm 2 */
  mn_gf_set_entry((byte *)"a2_e_chk",ssi_a2_e_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a2_t_chk",ssi_a2_t_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a2_c1",ssi_a2_c1);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_c2",ssi_a2_c2);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_sspd",ssi_a2_sspd);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_ic_evt",ssi_a2_ic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_sic_evt",ssi_a2_sic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_ri",ssi_a2_ri);      /*60 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a2_to",ssi_a2_to);      /*61 Alarm1 Recipient*/
  mn_gf_set_entry((byte *)"a2_msg",ssi_a2_msg);    /*62 Alarm1 Message  */

  /* Alarm 3 */
  mn_gf_set_entry((byte *)"a3_e_chk",ssi_a3_e_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a3_t_chk",ssi_a3_t_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a3_c1",ssi_a3_c1);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_c2",ssi_a3_c2);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_sspd",ssi_a3_sspd);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_ic_evt",ssi_a3_ic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_sic_evt",ssi_a3_sic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_ri",ssi_a3_ri);      /*60 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a3_to",ssi_a3_to);      /*61 Alarm1 Recipient*/
  mn_gf_set_entry((byte *)"a3_msg",ssi_a3_msg);    /*62 Alarm1 Message  */

  /* Alarm 4 */
  mn_gf_set_entry((byte *)"a4_e_chk",ssi_a4_e_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a4_t_chk",ssi_a4_t_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a4_c1",ssi_a4_c1);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_c2",ssi_a4_c2);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_sspd",ssi_a4_sspd);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_ic_evt",ssi_a4_ic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_sic_evt",ssi_a4_sic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_ri",ssi_a4_ri);      /*60 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a4_to",ssi_a4_to);      /*61 Alarm1 Recipient*/
  mn_gf_set_entry((byte *)"a4_msg",ssi_a4_msg);    /*62 Alarm1 Message  */

 /* Alarm 5 */
  mn_gf_set_entry((byte *)"a5_e_chk",ssi_a5_e_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a5_t_chk",ssi_a5_t_chk); /*47 Alarm1 Enable?*/
  mn_gf_set_entry((byte *)"a5_c1",ssi_a5_c1);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_c2",ssi_a5_c2);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_sspd",ssi_a5_sspd);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_ic_evt",ssi_a5_ic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_sic_evt",ssi_a5_sic_evt);   /*59 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_ri",ssi_a5_ri);      /*60 Alarm1 Trap  IPC*/
  mn_gf_set_entry((byte *)"a5_to",ssi_a5_to);      /*61 Alarm1 Recipient*/
  mn_gf_set_entry((byte *)"a5_msg",ssi_a5_msg);    /*62 Alarm1 Message  */

  mn_gf_set_entry((byte *)"dev_all", dev_all_func);/*63 Get all devs    */
  mn_gf_set_entry((byte *)"show_unit_num", show_unit_num_func);/*64 Unt#*/
  mn_gf_set_entry((byte *)"show_idnum", show_idnum_func);/*65 Show ID Nm*/
  mn_gf_set_entry((byte *)"show_name", show_name_func);/*66 Show Name Nm*/
  mn_gf_set_entry((byte *)"show_cmd", show_cmd_func);  /*67 Show CMD    */
  mn_gf_set_entry((byte *)"show_units", show_units_func); /*68 Show Unit*/
  

    /*------------- read_device.htm (Read all Devices in a table -------*/
  mn_gf_set_entry((byte *)"dev_read", dev_read_func);/* 69Read all Devs */ 
  mn_gf_set_entry((byte *)"chkd_devs", ssi_chkd_devs);/*70Read all Devs */
  mn_gf_set_entry((byte *)"timeout_select", ssi_timeout_select); /* 71  */
  mn_gf_set_entry((byte *)"load_timeout", ssi_load_timeout);     /* 72  */
  mn_gf_set_entry((byte *)"load_on",ssi_load_on);                /* 73  */
  //mn_gf_set_entry((byte *)"MAX_read", MAX_read_func); /* 71Read MAX devs*/
  //mn_gf_set_entry((byte *)"timeout_select",timeout_select_func);/* 72TM */
  //mn_gf_set_entry((byte *)"load_timeout",load_timeout_func);/* 73TM Load*/
  

  /*--------------- iopins.htm (GPIO Page) -----------------------------*/
  mn_gf_set_entry((byte *)"rs485txrx",ssi_rs485txrx);             /* 75 */
  mn_gf_set_entry((byte *)"cio",ssi_cio);                         /* 78 */
  mn_gf_set_entry((byte *)"s_no",ssi_s_no);                       /* 78 */
  mn_gf_set_entry((byte *)"i_chk",ssi_i_chk);                     /* 79 */
  mn_gf_set_entry((byte *)"fio_status",ssi_fio_status);               /* 79 */  
  mn_gf_set_entry((byte *)"o_chk",ssi_o_chk);                     /* 80 */
  mn_gf_set_entry((byte *)"h_chk",ssi_h_chk);                     /* 79 */
  mn_gf_set_entry((byte *)"l_chk",ssi_l_chk);                     /* 79 */
  mn_gf_set_entry((byte *)"image_source",ssi_image_source);       /* 79 */
  mn_gf_set_entry((byte *)"rs485timer",ssi_rs485timer);           /* 81 */
  mn_gf_set_entry((byte *)"rs485type",ssi_rs485type);           /* 81 */
 
  mn_gf_set_entry((byte *)"cio_status",ssi_cio_status);               /* 79 */
  mn_gf_set_entry((byte *)"ci_i_chk",ssi_ci_i_chk);               /* 79 */
  mn_gf_set_entry((byte *)"ci_o_chk",ssi_ci_o_chk);               /* 79 */
  mn_gf_set_entry((byte *)"ct_i_chk",ssi_ct_i_chk);               /* 79 */
  mn_gf_set_entry((byte *)"ct_o_chk",ssi_ct_o_chk);               /* 79 */

  /*--------------- device_query.htm (Device Query Page) ---------------*/
  mn_gf_set_entry((byte *)"response",ssi_response);               /* 82 */

  /*--------------- diagnostics.htm (Diagnostics page) -----------------*/
  
  mn_gf_set_entry((byte *)"fullserial",ssi_fullserial);           /* 83 */

  mn_gf_set_entry((byte *)"cts",ssi_cts);                         /* 83 */
  mn_gf_set_entry((byte *)"rts",ssi_rts);                         /* 83 */
  mn_gf_set_entry((byte *)"dsr",ssi_dsr);                         /* 83 */
  mn_gf_set_entry((byte *)"dcd",ssi_dcd);                         /* 83 */
  mn_gf_set_entry((byte *)"dtr",ssi_dtr);                         /* 83 */

  mn_gf_set_entry((byte *)"rx_bytes",ssi_rx_bytes);               /* 83 */
  mn_gf_set_entry((byte *)"tx_bytes",ssi_tx_bytes);               /* 84 */
  mn_gf_set_entry((byte *)"tcp_rx_bytes",ssi_tcp_rx_bytes);       /* 85 */
  mn_gf_set_entry((byte *)"tcp_tx_bytes",ssi_tcp_tx_bytes);       /* 86 */  
  mn_gf_set_entry((byte *)"udp_rx_bytes",ssi_udp_rx_bytes);       /* 87 */
  mn_gf_set_entry((byte *)"udp_tx_bytes",ssi_udp_tx_bytes);       /* 88 */
  mn_gf_set_entry((byte *)"icmp_rx_bytes",ssi_icmp_rx_bytes);     /* 89 */
  mn_gf_set_entry((byte *)"icmp_tx_bytes",ssi_icmp_tx_bytes);     /* 90 */
}
