#include "base.h"
#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

#define STATES {\
                CHOOSE(AUTH_REQ),\
                CHOOSE(AUTH_FAIL),\
                CHOOSE(AUTH_PASS)\
}
#define MAX_AUTHS 10

#define GET_STATE_NAME RPP_auth_get_state_name 
#include "choose.h"

int auth_packet_parse(rppPort_t *this, RPP_PDU_T *pdu)
{
    switch(pdu->body.msg.auth.type)
    {
        case MSG_AUTH_REQ:
            log_info("port %d recieve a packet for auth req",this->port_index);
            tx_auth_ack(this);
            break;
        case MSG_AUTH_ACK:
            log_info("port %d recieve a packet for auth ack",this->port_index);
            // 添加邻居信息
            memcpy(this->neighber_mac,pdu->body.sys_mac,MAC_LEN);
            this->neighber_port     = pdu->body.port_id;
            this->neighber_vaild    = 1;
            this->flag.f_auth       = AUTH_PASS;
            // 失效认证定时器
            RPP_OUT_close_timer(&this->auth_timer);
            break;
        default:
            return -4;
    }

    return 0;
}
void auth_timer_handler(struct uloop_timeout *timer)
{
    rppPort_t * port = rpp_get_port_addr(timer,auth_timer);
    
    if(port->auth_count++ < MAX_AUTHS)
    {
        tx_auth_req(port);
        RPP_OUT_set_timer(timer,RPP_OUT_get_time(port->owner,AUTH_TIME));
    }
    else
    {
        port->flag.f_auth = AUTH_FAIL;
    }

	RPP_ring_update (port->owner);
}

void RPP_auth_enter_state(STATE_MACH_T* this)
{
    register rppPort_t *port = this->owner.port;

    switch(this->state)
    {
        case BEGIN:
            RPP_OUT_close_timer(&port->auth_timer);
            port->flag.f_auth   = 0;
            break;
        case AUTH_REQ:
            RPP_OUT_set_timer(&port->auth_timer,RPP_OUT_get_time(port->owner,AUTH_TIME));
            log_info("  \033[1;33m<auth machine> port %d in req status!\033[0m",port->port_index);
            break;
        case AUTH_FAIL:
            log_info("  \033[1;33m<auth machine> port %d in fail status!\033[0m",port->port_index);
            break;
        case AUTH_PASS:
            log_info("  \033[1;33m<auth machine> port %d in pass status!\033[0m",port->port_index);
            break;
    }
}

Bool RPP_auth_check_conditions(STATE_MACH_T* this)
{
    register rppPort_t *port = this->owner.port;

    switch(this->state)
    {
        case BEGIN:
            if(port->link_st == LINK_UP) 
                return RPP_hop_2_state(this,AUTH_REQ);
            break;
        case AUTH_REQ:
            if(port->flag.f_auth == AUTH_FAIL)
                return RPP_hop_2_state(this,AUTH_FAIL);
            if(port->flag.f_auth == AUTH_PASS)
                return RPP_hop_2_state(this,AUTH_PASS);
            if(port->link_st == LINK_DOWN)
                return RPP_hop_2_state(this,BEGIN);
            break;
        case AUTH_FAIL:
            if(port->link_st == LINK_DOWN)
                return RPP_hop_2_state(this,BEGIN);
            break;
        case AUTH_PASS:
            if(port->link_st == LINK_DOWN)
                return RPP_hop_2_state(this,BEGIN);
            break;
        default:
            LOG_ERROR("auth machine in error status!\n");
            break;
    }

    return False;
}

