/*
 * AP mode helper functions
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "common/ieee802_11_defs.h"
#include "sta_info.h"
#include "hostapd.h"


int hostapd_register_probereq_cb(struct hostapd_data *hapd,
				 int (*cb)(void *ctx, const u8 *sa,
					   const u8 *ie, size_t ie_len),
				 void *ctx)
{
	struct hostapd_probereq_cb *n;

	n = os_realloc(hapd->probereq_cb, (hapd->num_probereq_cb + 1) *
		       sizeof(struct hostapd_probereq_cb));
	if (n == NULL)
		return -1;

	hapd->probereq_cb = n;
	n = &hapd->probereq_cb[hapd->num_probereq_cb];
	hapd->num_probereq_cb++;

	n->cb = cb;
	n->ctx = ctx;

	return 0;
}


struct prune_data {
	struct hostapd_data *hapd;
	const u8 *addr;
};

static int prune_associations(struct hostapd_iface *iface, void *ctx)
{
	struct prune_data *data = ctx;
	struct sta_info *osta;
	struct hostapd_data *ohapd;
	size_t j;

	for (j = 0; j < iface->num_bss; j++) {
		ohapd = iface->bss[j];
		if (ohapd == data->hapd)
			continue;
		osta = ap_get_sta(ohapd, data->addr);
		if (!osta)
			continue;

		ap_sta_disassociate(ohapd, osta, WLAN_REASON_UNSPECIFIED);
	}

	return 0;
}

/**
 * hostapd_prune_associations - Remove extraneous associations
 * @hapd: Pointer to BSS data for the most recent association
 * @addr: Associated STA address
 *
 * This function looks through all radios and BSS's for previous
 * (stale) associations of STA. If any are found they are removed.
 * 字面意思是删除当前接口下一些无关联系
 */
void hostapd_prune_associations(struct hostapd_data *hapd, const u8 *addr)
{
	struct prune_data data;
	data.hapd = hapd;
	data.addr = addr;
	if (hapd->iface->for_each_interface)
		hapd->iface->for_each_interface(hapd->iface->interfaces,
						prune_associations, &data);
}
