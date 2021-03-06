/*
 * Copyright (c) 2011-2019 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file csr_neighbor_roam.h
 *
 * Exports and types for the neighbor roaming algorithm which is sepcifically
 * designed for Android.
 */

#ifndef CSR_NEIGHBOR_ROAM_H
#define CSR_NEIGHBOR_ROAM_H

#include "sme_api.h"

#define ROAM_AP_AGE_LIMIT_MS                     10000

/* Enumeration of various states in neighbor roam algorithm */
typedef enum {
	eCSR_NEIGHBOR_ROAM_STATE_CLOSED,
	eCSR_NEIGHBOR_ROAM_STATE_INIT,
	eCSR_NEIGHBOR_ROAM_STATE_CONNECTED,
	eCSR_NEIGHBOR_ROAM_STATE_REASSOCIATING,
	eCSR_NEIGHBOR_ROAM_STATE_PREAUTHENTICATING,
	eCSR_NEIGHBOR_ROAM_STATE_PREAUTH_DONE,
	eNEIGHBOR_STATE_MAX
} eCsrNeighborRoamState;

/* Parameters that are obtained from CFG */
typedef struct sCsrNeighborRoamCfgParams {
	uint32_t neighborScanPeriod;
	uint32_t neighbor_scan_min_period;
	tCsrChannelInfo specific_chan_info;
	uint8_t neighborLookupThreshold;
	int8_t rssi_thresh_offset_5g;
	uint8_t neighborReassocThreshold;
	uint32_t minChannelScanTime;
	uint32_t maxChannelScanTime;
	uint16_t neighborResultsRefreshPeriod;
	uint16_t emptyScanRefreshPeriod;
	uint8_t nOpportunisticThresholdDiff;
	uint8_t nRoamRescanRssiDiff;
	uint8_t nRoamBmissFirstBcnt;
	uint8_t nRoamBmissFinalBcnt;
	uint8_t nRoamBeaconRssiWeight;
	uint8_t delay_before_vdev_stop;
	uint32_t hi_rssi_scan_max_count;
	uint32_t hi_rssi_scan_rssi_delta;
	uint32_t hi_rssi_scan_delay;
	int32_t hi_rssi_scan_rssi_ub;
	tCsrChannelInfo pref_chan_info;
	uint32_t full_roam_scan_period;
	bool enable_scoring_for_roam;
	uint8_t roam_rssi_diff;
	uint16_t roam_scan_home_away_time;
	uint8_t roam_scan_n_probes;
	uint32_t roam_scan_inactivity_time;
	uint32_t roam_inactive_data_packet_count;
	uint32_t roam_scan_period_after_inactivity;
} tCsrNeighborRoamCfgParams, *tpCsrNeighborRoamCfgParams;

#define CSR_NEIGHBOR_ROAM_INVALID_CHANNEL_INDEX    255
typedef struct sCsrNeighborRoamChannelInfo {
	/* Flag to mark reception of IAPP Neighbor list */
	bool IAPPNeighborListReceived;
	/* Current channel index that is being scanned */
	uint8_t currentChanIndex;
	/* Max number of channels in channel list and the list of channels */
	tCsrChannelInfo currentChannelListInfo;
} tCsrNeighborRoamChannelInfo, *tpCsrNeighborRoamChannelInfo;

typedef struct sCsrNeighborRoamBSSInfo {
	tListElem List;
	uint8_t apPreferenceVal;
	struct bss_description *pBssDescription;
} tCsrNeighborRoamBSSInfo, *tpCsrNeighborRoamBSSInfo;

#define CSR_NEIGHBOR_ROAM_REPORT_QUERY_TIMEOUT  1000       /* in milliseconds */
/* Max number of MAC addresses with which the pre-auth was failed */
#define MAX_NUM_PREAUTH_FAIL_LIST_ADDRESS          10
#define CSR_NEIGHBOR_ROAM_MAX_NUM_PREAUTH_RETRIES  3

/* Black listed APs. List of MAC Addresses with which the Preauth was failed */
typedef struct sCsrPreauthFailListInfo {
	uint8_t numMACAddress;
	tSirMacAddr macAddress[MAX_NUM_PREAUTH_FAIL_LIST_ADDRESS];
} tCsrPreauthFailListInfo, *tpCsrPreauthFailListInfo;

typedef struct sCsrNeighborReportBssInfo {
	uint8_t channelNum;
	uint8_t neighborScore;
	tSirMacAddr neighborBssId;
} tCsrNeighborReportBssInfo, *tpCsrNeighborReportBssInfo;

typedef struct sCsr11rAssocNeighborInfo {
	bool preauthRspPending;
	bool neighborRptPending;
	uint8_t currentNeighborRptRetryNum;
	tCsrPreauthFailListInfo preAuthFailList;
	uint32_t neighborReportTimeout;
	uint8_t numPreAuthRetries;
	tDblLinkList preAuthDoneList;   /* llist which consists/preauth nodes */
} tCsr11rAssocNeighborInfo, *tpCsr11rAssocNeighborInfo;

/**
 * struct sCsr11rAssocNeighborInfo - Control info for neighbor roam algorithm
 * @roam_control_enable: Flag used to cache the status of roam control
 *			 configuration. This will be set only if the
 *			 corresponding vendor command data is configured to
 *			 driver/firmware successfully. The same shall be
 *			 returned to userspace whenever queried for roam
 *			 control config status.
 */
typedef struct sCsrNeighborRoamControlInfo {
	eCsrNeighborRoamState neighborRoamState;
	eCsrNeighborRoamState prevNeighborRoamState;
	tCsrNeighborRoamCfgParams cfgParams;
	struct qdf_mac_addr currAPbssid;  /* current assoc AP */
	uint8_t currAPoperationChannel; /* current assoc AP */
	tCsrNeighborRoamChannelInfo roamChannelInfo;
	uint8_t currentNeighborLookupThreshold;
	uint8_t currentOpportunisticThresholdDiff;
	uint8_t currentRoamRescanRssiDiff;
	tDblLinkList roamableAPList;    /* List of current FT candidates */
	struct csr_roam_profile csrNeighborRoamProfile;
	bool is11rAssoc;
	tCsr11rAssocNeighborInfo FTRoamInfo;
#ifdef FEATURE_WLAN_ESE
	bool isESEAssoc;
	bool isVOAdmitted;
	uint16_t MinQBssLoadRequired;
#endif
	/*
	 * Previous connected profile.
	 * If the new profile does not match previous we re-initialize
	 * occupied channel list
	 */
	tCsrRoamConnectedProfile prevConnProfile;
	/* upper layer requested a reassoc */
	uint8_t uOsRequestedHandoff;
	/* handoff related info came with upper layer's req for reassoc */
	tCsrHandoffRequest handoffReqInfo;
	uint8_t currentRoamBmissFirstBcnt;
	uint8_t currentRoamBmissFinalBcnt;
	uint8_t currentRoamBeaconRssiWeight;
	uint8_t last_sent_cmd;
	struct scan_result_list *scan_res_lfr2_roam_ap;
	bool roam_control_enable;
} tCsrNeighborRoamControlInfo, *tpCsrNeighborRoamControlInfo;

/* All the necessary Function declarations are here */
QDF_STATUS csr_neighbor_roam_indicate_connect(struct mac_context *mac,
		uint8_t sessionId, QDF_STATUS status);
QDF_STATUS csr_neighbor_roam_indicate_disconnect(struct mac_context *mac,
		uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_init(struct mac_context *mac, uint8_t sessionId);
void csr_neighbor_roam_close(struct mac_context *mac, uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_prepare_scan_profile_filter(struct mac_context *mac,
		tCsrScanResultFilter *pScanFilter, uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_preauth_rsp_handler(struct mac_context *mac,
		uint8_t sessionId, QDF_STATUS limStatus);
bool csr_neighbor_roam_is11r_assoc(struct mac_context *mac, uint8_t sessionId);
#ifdef WLAN_FEATURE_HOST_ROAM
void csr_neighbor_roam_tranistion_preauth_done_to_disconnected(
		struct mac_context *mac, uint8_t sessionId);
bool csr_neighbor_roam_state_preauth_done(struct mac_context *mac,
		uint8_t sessionId);
QDF_STATUS csr_roam_issue_reassociate_cmd(struct mac_context *mac,
		uint32_t sessionId);
void csr_neighbor_roam_free_roamable_bss_list(struct mac_context *mac_ctx,
		tDblLinkList *llist);
bool csr_neighbor_roam_get_handoff_ap_info(struct mac_context *mac,
		tpCsrNeighborRoamBSSInfo pHandoffNode, uint8_t sessionId);
QDF_STATUS csr_roam_issue_reassociate(struct mac_context *mac,
		uint32_t sessionId, struct bss_description *pSirBssDesc,
		tDot11fBeaconIEs *pIes, struct csr_roam_profile *pProfile);
void csr_neighbor_roam_request_handoff(struct mac_context *mac, uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_candidate_found_ind_hdlr(struct mac_context *mac,
		void *pMsg);
QDF_STATUS csr_neighbor_roam_process_scan_complete(struct mac_context *mac,
		uint8_t sessionId);
bool csr_neighbor_roam_is_handoff_in_progress(struct mac_context *mac,
		uint8_t sessionId);
void csr_neighbor_roam_reset_preauth_control_info(
		struct mac_context *mac_ctx, uint8_t session_id);
void csr_neighbor_roam_purge_preauth_failed_list(struct mac_context *mac);
#else
static inline bool csr_neighbor_roam_state_preauth_done(struct mac_context *mac,
		uint8_t sessionId)
{
	return false;
}
static inline QDF_STATUS csr_roam_issue_reassociate_cmd(struct mac_context *mac,
		uint32_t sessionId)
{
	return QDF_STATUS_E_NOSUPPORT;
}
static inline QDF_STATUS csr_roam_issue_reassociate(struct mac_context *mac,
		uint32_t sessionId, struct bss_description *pSirBssDesc,
		tDot11fBeaconIEs *pIes, struct csr_roam_profile *pProfile)
{
	return QDF_STATUS_E_NOSUPPORT;
}
static inline QDF_STATUS csr_neighbor_roam_candidate_found_ind_hdlr(
		struct mac_context *mac, void *pMsg)
{
	return QDF_STATUS_E_NOSUPPORT;
}
static inline QDF_STATUS csr_neighbor_roam_process_scan_complete(
		struct mac_context *mac, uint8_t sessionId)
{
	return QDF_STATUS_E_NOSUPPORT;
}
static inline void csr_neighbor_roam_tranistion_preauth_done_to_disconnected(
		struct mac_context *mac, uint8_t sessionId)
{}
static inline void csr_neighbor_roam_free_roamable_bss_list(
		struct mac_context *mac_ctx, tDblLinkList *llist)
{}
static inline void csr_neighbor_roam_request_handoff(struct mac_context *mac,
		uint8_t sessionId)
{}
static inline void csr_neighbor_roam_reset_preauth_control_info(
		struct mac_context *mac_ctx, uint8_t session_id)
{}
static inline void csr_neighbor_roam_purge_preauth_failed_list(
		struct mac_context *mac)
{}
static inline bool csr_neighbor_roam_get_handoff_ap_info(struct mac_context *mac,
		tpCsrNeighborRoamBSSInfo pHandoffNode, uint8_t sessionId)
{
	return false;
}
static inline bool csr_neighbor_roam_is_handoff_in_progress(struct mac_context *mac,
		uint8_t sessionId)
{
	return false;
}
#endif
bool csr_neighbor_middle_of_roaming(struct mac_context *mac, uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_update_config(struct mac_context *mac_ctx,
		uint8_t session_id, uint8_t value, uint8_t reason);
QDF_STATUS csr_neighbor_roam_update_fast_roaming_enabled(struct mac_context *mac,
		uint8_t sessionId, const bool fastRoamEnabled);
QDF_STATUS csr_neighbor_roam_channels_filter_by_current_band(
		struct mac_context *mac, uint8_t sessionId,
		uint8_t *pInputChannelList,
		uint8_t inputNumOfChannels,
		uint8_t *pOutputChannelList,
		uint8_t *pMergedOutputNumOfChannels);
QDF_STATUS csr_neighbor_roam_merge_channel_lists(struct mac_context *mac,
		uint8_t *pInputChannelList,
		uint8_t inputNumOfChannels,
		uint8_t *pOutputChannelList,
		uint8_t outputNumOfChannels,
		uint8_t *pMergedOutputNumOfChannels);
void csr_roam_reset_roam_params(struct mac_context *mac_ptr);
#define ROAM_SCAN_OFFLOAD_START                     1
#define ROAM_SCAN_OFFLOAD_STOP                      2
#define ROAM_SCAN_OFFLOAD_RESTART                   3
#define ROAM_SCAN_OFFLOAD_UPDATE_CFG                4
#define ROAM_SCAN_OFFLOAD_ABORT_SCAN                5

#define REASON_CONNECT                              1
#define REASON_CHANNEL_LIST_CHANGED                 2
#define REASON_LOOKUP_THRESH_CHANGED                3
#define REASON_DISCONNECTED                         4
#define REASON_RSSI_DIFF_CHANGED                    5
#define REASON_ESE_INI_CFG_CHANGED                  6
#define REASON_NEIGHBOR_SCAN_REFRESH_PERIOD_CHANGED 7
#define REASON_VALID_CHANNEL_LIST_CHANGED           8
#define REASON_FLUSH_CHANNEL_LIST                   9
#define REASON_EMPTY_SCAN_REF_PERIOD_CHANGED        10
#define REASON_PREAUTH_FAILED_FOR_ALL               11
#define REASON_NO_CAND_FOUND_OR_NOT_ROAMING_NOW     12
#define REASON_NPROBES_CHANGED                      13
#define REASON_HOME_AWAY_TIME_CHANGED               14
#define REASON_OS_REQUESTED_ROAMING_NOW             15
#define REASON_SCAN_CH_TIME_CHANGED                 16
#define REASON_SCAN_HOME_TIME_CHANGED               17
#define REASON_OPPORTUNISTIC_THRESH_DIFF_CHANGED    18
#define REASON_ROAM_RESCAN_RSSI_DIFF_CHANGED        19
#define REASON_ROAM_BMISS_FIRST_BCNT_CHANGED        20
#define REASON_ROAM_BMISS_FINAL_BCNT_CHANGED        21
#define REASON_ROAM_BEACON_RSSI_WEIGHT_CHANGED      22
#define REASON_ROAM_DFS_SCAN_MODE_CHANGED           23
#define REASON_ROAM_ABORT_ROAM_SCAN                 24
#define REASON_ROAM_EXT_SCAN_PARAMS_CHANGED         25
#define REASON_ROAM_SET_SSID_ALLOWED                26
#define REASON_ROAM_SET_FAVORED_BSSID               27
#define REASON_ROAM_GOOD_RSSI_CHANGED               28
#define REASON_ROAM_SET_BLACKLIST_BSSID             29
#define REASON_ROAM_SCAN_HI_RSSI_MAXCOUNT_CHANGED   30
#define REASON_ROAM_SCAN_HI_RSSI_DELTA_CHANGED      31
#define REASON_ROAM_SCAN_HI_RSSI_DELAY_CHANGED      32
#define REASON_ROAM_SCAN_HI_RSSI_UB_CHANGED         33
#define REASON_CONNECT_IES_CHANGED                  34
#define REASON_ROAM_SCAN_STA_ROAM_POLICY_CHANGED    35
#define REASON_ROAM_SYNCH_FAILED                    36
#define REASON_ROAM_PSK_PMK_CHANGED                 37
#define REASON_ROAM_STOP_ALL                        38
#define REASON_SUPPLICANT_DISABLED_ROAMING          39
#define REASON_CTX_INIT                             40
#define REASON_FILS_PARAMS_CHANGED                  41
#define REASON_SME_ISSUED                           42
#define REASON_DRIVER_ENABLED                       43
#define REASON_ROAM_FULL_SCAN_PERIOD_CHANGED        44
#define REASON_SCORING_CRITERIA_CHANGED             45
#define REASON_SUPPLICANT_INIT_ROAMING              46
#define REASON_SUPPLICANT_DE_INIT_ROAMING           47
#define REASON_DRIVER_DISABLED                      48
#define REASON_ROAM_CONTROL_CONFIG_RESTORED         49
#define REASON_ROAM_CONTROL_CONFIG_ENABLED          50

#if defined(WLAN_FEATURE_HOST_ROAM) || defined(WLAN_FEATURE_ROAM_OFFLOAD)
QDF_STATUS csr_roam_offload_scan(struct mac_context *mac, uint8_t sessionId,
		uint8_t command, uint8_t reason);

/**
 * csr_post_roam_state_change() - Post roam state change to roam state machine
 * @mac: mac context
 * @vdev_id: vdev id
 * @state: roam state to be set for the requested vdev id
 * @reason: reason for changing roam state for the requested vdev id
 *
 * Return: QDF_STATUS
 */
QDF_STATUS csr_post_roam_state_change(struct mac_context *mac, uint8_t vdev_id,
				      enum roam_offload_state state,
				      uint8_t reason);

/**
 * csr_post_rso_stop() - Post RSO stop message to WMA
 * @mac: mac context
 * @vdev_id: vdev id
 * @reason: reason for requesting RSO stop
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
csr_post_rso_stop(struct mac_context *mac, uint8_t vdev_id, uint16_t reason);

/**
 * csr_enable_roaming_on_connected_sta() - Enable roaming on other connected
 *  sta vdev
 * @mac: mac context
 * @vdev_id: vdev id on which roaming should not be enabled
 * @reason: reason for enabling roaming on connected sta vdev
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
csr_enable_roaming_on_connected_sta(struct mac_context *mac, uint8_t vdev_id);

/**
 * csr_roam_update_cfg() - Process RSO update cfg request
 * @mac: mac context
 * @vdev_id: vdev id
 * @reason: reason for requesting RSO update cfg
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
csr_roam_update_cfg(struct mac_context *mac, uint8_t vdev_id, uint8_t reason);
#else
static inline QDF_STATUS csr_roam_offload_scan(struct mac_context *mac,
		uint8_t sessionId, uint8_t command, uint8_t reason)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS csr_post_roam_state_change(struct mac_context *mac, uint8_t vdev_id,
				      enum roam_offload_state state,
				      uint8_t reason)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
csr_post_rso_stop(struct mac_context *mac, uint8_t vdev_id, uint16_t reason)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline QDF_STATUS
csr_enable_roaming_on_connected_sta(struct mac_context *mac, uint8_t vdev_id)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline QDF_STATUS
csr_roam_update_cfg(struct mac_context *mac, uint8_t vdev_id, uint8_t reason)
{
	return QDF_STATUS_E_NOSUPPORT;
}
#endif

/**
 * csr_get_roam_enabled_sta_sessionid() - get the session id of the sta on which
 * roaming is enabled.
 * @mac_ctx:  pointer to global mac structure
 *
 * The function check if any sta is present and has roaming enabled and return
 * the session id of the sta with roaming enabled else if roaming is not enabled
 * on any STA return WLAN_UMAC_VDEV_ID_MAX
 *
 * Return: session id of STA on which roaming is enabled
 */
uint8_t csr_get_roam_enabled_sta_sessionid(
	struct mac_context *mac_ctx);

#if defined(WLAN_FEATURE_FILS_SK)
/**
 * csr_update_fils_config - Update FILS config to CSR roam session
 * @mac: MAC context
 * @session_id: session id
 * @src_profile: Source profile having latest FILS config
 *
 * API to update FILS config to roam csr session
 *
 * Return: QDF_STATUS
 */
QDF_STATUS csr_update_fils_config(struct mac_context *mac, uint8_t session_id,
				  struct csr_roam_profile *src_profile);
#endif

QDF_STATUS csr_neighbor_roam_handoff_req_hdlr(struct mac_context *mac, void *pMsg);
QDF_STATUS csr_neighbor_roam_proceed_with_handoff_req(struct mac_context *mac,
		uint8_t sessionId);
QDF_STATUS csr_neighbor_roam_sssid_scan_done(struct mac_context *mac,
		uint8_t sessionId, QDF_STATUS status);
QDF_STATUS csr_neighbor_roam_start_lfr_scan(struct mac_context *mac,
		uint8_t sessionId);

#ifdef FEATURE_WLAN_ESE
QDF_STATUS csr_set_cckm_ie(struct mac_context *mac, const uint8_t sessionId,
		const uint8_t *pCckmIe, const uint8_t ccKmIeLen);
QDF_STATUS csr_roam_read_tsf(struct mac_context *mac, uint8_t *pTimestamp,
		const uint8_t sessionId);
#endif /* FEATURE_WLAN_ESE */
#ifdef WLAN_FEATURE_ROAM_OFFLOAD
QDF_STATUS csr_roam_synch_callback(struct mac_context *mac,
	struct roam_offload_synch_ind *roam_synch_data,
	struct bss_description *bss_desc_ptr, enum sir_roam_op_code reason);

/**
 * csr_roam_auth_offload_callback() - Registered CSR Callback function to handle
 * WPA3 roam pre-auth event from firmware.
 * @mac_ctx: Global mac context pointer
 * @vdev_id: Vdev id
 * @bssid: candidate AP bssid
 */
QDF_STATUS
csr_roam_auth_offload_callback(struct mac_context *mac_ctx,
			       uint8_t vdev_id,
			       struct qdf_mac_addr bssid);

/**
 * csr_fast_reassoc() - invokes FAST REASSOC command
 * @mac_handle: handle returned by mac_open
 * @profile: current connected profile
 * @bssid: bssid to look for in scan cache
 * @channel: channel on which reassoc should be send
 * @vdev_id: vdev id
 * @connected_bssid: bssid of currently connected profile
 *
 * Return: QDF_STATUS
 */
QDF_STATUS csr_fast_reassoc(mac_handle_t mac_handle,
			    struct csr_roam_profile *profile,
			    const tSirMacAddr bssid, int channel,
			    uint8_t vdev_id, const tSirMacAddr connected_bssid);

/**
 * csr_process_roam_auth_offload_callback() - API to trigger the
 * WPA3 pre-auth event for candidate AP received from firmware.
 * @vdev_id: vdev id
 * @roam_bssid: Candidate BSSID to roam
 *
 * This function calls the hdd_sme_roam_callback with reason
 * eCSR_ROAM_SAE_COMPUTE to trigger SAE auth to supplicant.
 */
QDF_STATUS
csr_process_roam_auth_offload_callback(struct mac_context *mac_ctx,
				       uint8_t vdev_id,
				       struct qdf_mac_addr roam_bssid);
#else
static inline QDF_STATUS csr_roam_synch_callback(struct mac_context *mac,
	struct roam_offload_synch_ind *roam_synch_data,
	struct bss_description *bss_desc_ptr, enum sir_roam_op_code reason)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS csr_fast_reassoc(mac_handle_t mac_handle,
			    struct csr_roam_profile *profile,
			    const tSirMacAddr bssid, int channel,
			    uint8_t vdev_id, const tSirMacAddr connected_bssid)
{
	return QDF_STATUS_SUCCESS;
}

static inline QDF_STATUS
csr_roam_auth_offload_callback(struct mac_context *mac_ctx,
			       uint8_t vdev_id,
			       struct qdf_mac_addr bssid)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline QDF_STATUS
csr_process_roam_auth_offload_callback(struct mac_context *mac_ctx,
				       uint8_t vdev_id,
				       struct qdf_mac_addr roam_bssid)
{
	return QDF_STATUS_E_NOSUPPORT;
}
#endif
void csr_neighbor_roam_state_transition(struct mac_context *mac_ctx,
		uint8_t newstate, uint8_t session);
uint8_t *csr_neighbor_roam_state_to_string(uint8_t state);
tpCsrNeighborRoamBSSInfo csr_neighbor_roam_next_roamable_ap(
		struct mac_context *mac_ctx, tDblLinkList *llist,
		tpCsrNeighborRoamBSSInfo neighbor_entry);
bool csr_neighbor_roam_remove_roamable_ap_list_entry(struct mac_context *mac,
		tDblLinkList *pList, tpCsrNeighborRoamBSSInfo pNeighborEntry);
void csr_neighbor_roam_free_neighbor_roam_bss_node(struct mac_context *mac,
		tpCsrNeighborRoamBSSInfo neighborRoamBSSNode);
QDF_STATUS csr_neighbor_roam_issue_preauth_req(struct mac_context *mac,
		uint8_t sessionId);
bool csr_neighbor_roam_is_preauth_candidate(struct mac_context *mac,
		    uint8_t sessionId, tSirMacAddr bssId);
#ifdef FEATURE_WLAN_LFR_METRICS
void csr_neighbor_roam_send_lfr_metric_event(struct mac_context *mac_ctx,
		uint8_t session_id, tSirMacAddr bssid, eRoamCmdStatus status);
#else
static inline void csr_neighbor_roam_send_lfr_metric_event(
		struct mac_context *mac_ctx, uint8_t session_id,
		tSirMacAddr bssid, eRoamCmdStatus status)
{}
#endif
QDF_STATUS csr_roam_stop_wait_for_key_timer(struct mac_context *mac);
QDF_STATUS csr_roam_copy_connected_profile(struct mac_context *mac,
		uint32_t sessionId, struct csr_roam_profile *pDstProfile);

/**
 * csr_invoke_neighbor_report_request - Send neighbor report invoke command to
 *					WMA
 * @mac_ctx: MAC context
 * @session_id: session id
 *
 * API called from IW to invoke neighbor report request to WMA then to FW
 *
 * Return: QDF_STATUS
 */
QDF_STATUS csr_invoke_neighbor_report_request(uint8_t session_id,
				struct sRrmNeighborReq *neighbor_report_req,
				bool send_resp_to_host);

#endif /* CSR_NEIGHBOR_ROAM_H */
