#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "interpret.h"

void protocol_http (Protocol * pro);
void protocol_ftp ();
void protocol_smtp ();
void protocol_dns ();
void protocol_tcp ();
void protocol_pop3 ();
void protocol_imap ();
void protocol_ssh ();
void protocol_snmp ();
void protocol_https ();
void protocol_sftp ();
void protocol_web_socket ();
void protocol_mttq ();
void protocol_udp ();
void define_protocol ();
Protocol * get_protocol (const char * name);
void * evalue_http_protocol (Protocol * pro, const char * member_name);

#endif

