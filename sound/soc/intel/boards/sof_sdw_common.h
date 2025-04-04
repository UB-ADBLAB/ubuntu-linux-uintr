/* SPDX-License-Identifier: GPL-2.0-only
 *  Copyright (c) 2020 Intel Corporation
 */

/*
 *  sof_sdw_common.h - prototypes for common helpers
 */

#ifndef SND_SOC_SOF_SDW_COMMON_H
#define SND_SOC_SOF_SDW_COMMON_H

#include <linux/bits.h>
#include <linux/types.h>
#include <sound/soc.h>
#include "sof_hdmi_common.h"

#define MAX_NO_PROPS 2
#define MAX_HDMI_NUM 4
#define SDW_UNUSED_DAI_ID -1
#define SDW_JACK_OUT_DAI_ID 0
#define SDW_JACK_IN_DAI_ID 1
#define SDW_AMP_OUT_DAI_ID 2
#define SDW_AMP_IN_DAI_ID 3
#define SDW_DMIC_DAI_ID 4
#define SDW_MAX_CPU_DAIS 16
#define SDW_INTEL_BIDIR_PDI_BASE 2

#define SDW_MAX_LINKS		4

/* 8 combinations with 4 links + unused group 0 */
#define SDW_MAX_GROUPS 9

enum {
	SOF_PRE_TGL_HDMI_COUNT = 3,
	SOF_TGL_HDMI_COUNT = 4,
};

enum {
	SOF_I2S_SSP0 = BIT(0),
	SOF_I2S_SSP1 = BIT(1),
	SOF_I2S_SSP2 = BIT(2),
	SOF_I2S_SSP3 = BIT(3),
	SOF_I2S_SSP4 = BIT(4),
	SOF_I2S_SSP5 = BIT(5),
};

#define SOF_JACK_JDSRC(quirk)		((quirk) & GENMASK(3, 0))
/* Deprecated and no longer supported by the code */
#define SOF_SDW_FOUR_SPK		BIT(4)
#define SOF_SDW_TGL_HDMI		BIT(5)
#define SOF_SDW_PCH_DMIC		BIT(6)
#define SOF_SSP_PORT(x)		(((x) & GENMASK(5, 0)) << 7)
#define SOF_SSP_GET_PORT(quirk)	(((quirk) >> 7) & GENMASK(5, 0))
/* Deprecated and no longer supported by the code */
#define SOF_SDW_NO_AGGREGATION		BIT(14)
/* If a CODEC has an optional speaker output, this quirk will enable it */
#define SOF_CODEC_SPKR			BIT(15)
/*
 * If the CODEC has additional devices attached directly to it.
 *
 * For the cs42l43:
 *   - 0 - No speaker output
 *   - SOF_CODEC_SPKR - CODEC internal speaker
 *   - SOF_SIDECAR_AMPS - 2x Sidecar amplifiers + CODEC internal speaker
 *   - SOF_CODEC_SPKR | SOF_SIDECAR_AMPS - Not currently supported
 */
#define SOF_SIDECAR_AMPS		BIT(16)
#define SOF_CODEC_MIC			BIT(17)

/* BT audio offload: reserve 3 bits for future */
#define SOF_BT_OFFLOAD_SSP_SHIFT	15
#define SOF_BT_OFFLOAD_SSP_MASK	(GENMASK(17, 15))
#define SOF_BT_OFFLOAD_SSP(quirk)	\
	(((quirk) << SOF_BT_OFFLOAD_SSP_SHIFT) & SOF_BT_OFFLOAD_SSP_MASK)
#define SOF_SSP_BT_OFFLOAD_PRESENT	BIT(18)

#define SOF_SDW_DAI_TYPE_JACK		0
#define SOF_SDW_DAI_TYPE_AMP		1
#define SOF_SDW_DAI_TYPE_MIC		2

#define SOF_SDW_MAX_DAI_NUM		8

struct sof_sdw_codec_info;

struct sof_sdw_dai_info {
	const bool direction[2]; /* playback & capture support */
	const char *dai_name;
	const int dai_type;
	const int dailink[2]; /* dailink id for each direction */
	const struct snd_kcontrol_new *controls;
	const int num_controls;
	const struct snd_soc_dapm_widget *widgets;
	const int num_widgets;
	int  (*init)(struct snd_soc_card *card,
		     struct snd_soc_dai_link *dai_links,
		     struct sof_sdw_codec_info *info,
		     bool playback);
	int (*exit)(struct snd_soc_card *card, struct snd_soc_dai_link *dai_link);
	int (*rtd_init)(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
	bool rtd_init_done; /* Indicate that the rtd_init callback is done */
	unsigned long quirk;
	bool quirk_exclude;
};

struct sof_sdw_codec_info {
	const int part_id;
	const int version_id;
	const char *codec_name;
	int amp_num;
	const u8 acpi_id[ACPI_ID_LEN];
	const bool ignore_pch_dmic;
	const struct snd_soc_ops *ops;
	struct sof_sdw_dai_info dais[SOF_SDW_MAX_DAI_NUM];
	const int dai_num;

	int (*codec_card_late_probe)(struct snd_soc_card *card);

	int  (*count_sidecar)(struct snd_soc_card *card,
			      int *num_dais, int *num_devs);
	int  (*add_sidecar)(struct snd_soc_card *card,
			    struct snd_soc_dai_link **dai_links,
			    struct snd_soc_codec_conf **codec_conf);
};

struct mc_private {
	struct snd_soc_card card;
	struct snd_soc_jack sdw_headset;
	struct sof_hdmi_private hdmi;
	struct device *headset_codec_dev; /* only one headset per card */
	struct device *amp_dev1, *amp_dev2;
	/* To store SDW Pin index for each SoundWire link */
	unsigned int sdw_pin_index[SDW_MAX_LINKS];
	bool append_dai_type;
	bool ignore_pch_dmic;
};

extern unsigned long sof_sdw_quirk;

int sdw_startup(struct snd_pcm_substream *substream);
int sdw_prepare(struct snd_pcm_substream *substream);
int sdw_trigger(struct snd_pcm_substream *substream, int cmd);
int sdw_hw_params(struct snd_pcm_substream *substream,
		  struct snd_pcm_hw_params *params);
int sdw_hw_free(struct snd_pcm_substream *substream);
void sdw_shutdown(struct snd_pcm_substream *substream);

/* generic HDMI support */
int sof_sdw_hdmi_init(struct snd_soc_pcm_runtime *rtd);

int sof_sdw_hdmi_card_late_probe(struct snd_soc_card *card);

/* DMIC support */
int sof_sdw_dmic_init(struct snd_soc_pcm_runtime *rtd);

/* RT711 support */
int sof_sdw_rt711_init(struct snd_soc_card *card,
		       struct snd_soc_dai_link *dai_links,
		       struct sof_sdw_codec_info *info,
		       bool playback);
int sof_sdw_rt711_exit(struct snd_soc_card *card, struct snd_soc_dai_link *dai_link);

/* RT711-SDCA support */
int sof_sdw_rt_sdca_jack_init(struct snd_soc_card *card,
			      struct snd_soc_dai_link *dai_links,
			      struct sof_sdw_codec_info *info,
			      bool playback);
int sof_sdw_rt_sdca_jack_exit(struct snd_soc_card *card, struct snd_soc_dai_link *dai_link);

/* RT1308 I2S support */
extern const struct snd_soc_ops sof_sdw_rt1308_i2s_ops;

/* generic amp support */
int sof_sdw_rt_amp_init(struct snd_soc_card *card,
			struct snd_soc_dai_link *dai_links,
			struct sof_sdw_codec_info *info,
			bool playback);
int sof_sdw_rt_amp_exit(struct snd_soc_card *card, struct snd_soc_dai_link *dai_link);

/* MAXIM codec support */
int sof_sdw_maxim_init(struct snd_soc_card *card,
		       struct snd_soc_dai_link *dai_links,
		       struct sof_sdw_codec_info *info,
		       bool playback);

/* CS42L43 support */
int sof_sdw_cs42l43_spk_init(struct snd_soc_card *card,
			     struct snd_soc_dai_link *dai_links,
			     struct sof_sdw_codec_info *info,
			     bool playback);

/* CS AMP support */
int bridge_cs35l56_count_sidecar(struct snd_soc_card *card,
				 int *num_dais, int *num_devs);
int bridge_cs35l56_add_sidecar(struct snd_soc_card *card,
			       struct snd_soc_dai_link **dai_links,
			       struct snd_soc_codec_conf **codec_conf);
int bridge_cs35l56_spk_init(struct snd_soc_card *card,
			    struct snd_soc_dai_link *dai_links,
			    struct sof_sdw_codec_info *info,
			    bool playback);

int sof_sdw_cs_amp_init(struct snd_soc_card *card,
			struct snd_soc_dai_link *dai_links,
			struct sof_sdw_codec_info *info,
			bool playback);

/* dai_link init callbacks */

int cs42l42_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int cs42l43_hs_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int cs42l43_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int cs42l43_dmic_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int cs_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int maxim_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt5682_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt700_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt711_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt712_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt722_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt_dmic_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt_amp_spk_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);
int rt_sdca_jack_rtd_init(struct snd_soc_pcm_runtime *rtd, struct snd_soc_dai *dai);

#endif
