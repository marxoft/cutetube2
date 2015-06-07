/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCALEMODEL_H
#define LOCALEMODEL_H

#include "selectionmodel.h"

class LocaleModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit LocaleModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("Afan"), "om_ET");
        append(tr("Afar"), "aa_DJ");
        append(tr("Afrikaans"), "af_ZA");
        append(tr("Akan"), "ak_GH");
        append(tr("Albanian"), "sq_AL");
        append(tr("Amharic"), "am_ET");
        append(tr("Arabic"), "ar_SA");
        append(tr("Armenian"), "hy_AM");
        append(tr("Assamese"), "as_IN");
        append(tr("Asu"), "asa_TZ");
        append(tr("Atsam"), "cch_NG");
        append(tr("Azerbaijani"), "az_AZ");
        append(tr("Bambara"), "bm_ML");
        append(tr("Basque"), "eu_ES");
        append(tr("Bemba"), "bem_ZM");
        append(tr("Bena"), "bez_TZ");
        append(tr("Bengali"), "bn_BD");
        append(tr("Bhutani"), "dz_BT");
        append(tr("Blin"), "byn_ER");
        append(tr("Bosnian"), "bs_BA");
        append(tr("Breton"), "br_FR");
        append(tr("Bulgarian"), "bg_BG");
        append(tr("Burmese"), "my_MM");
        append(tr("Byelorussian"), "be_BY");
        append(tr("Cambodian"), "km_KH");
        append(tr("Catalan"), "ca_ES");
        append(tr("Central Morocco Tamazight"), "tzm_MA");
        append(tr("Cherokee"), "chr_US");
        append(tr("Chiga"), "cgg_UG");
        append(tr("Chinese"), "zh_CN");
        append(tr("Colognian"), "ksh_DE");
        append(tr("Cornish"), "kw_GB");
        append(tr("Croatian"), "hr_HR");
        append(tr("Czech"), "cs_CZ");
        append(tr("Danish"), "da_DK");
        append(tr("Dutch"), "nl_NL");
        append(tr("Embu"), "ebu_KE");
        append(tr("English (GB)"), "en_GB");
        append(tr("English (US)"), "en_US");
        append(tr("Estonian"), "et_EE");
        append(tr("Ewe"), "ee_GH");
        append(tr("Faroese"), "fo_FO");
        append(tr("Filipino"), "fil_PH");
        append(tr("Finnish"), "fi_FI");
        append(tr("French"), "fr_FR");
        append(tr("Friulian"), "fur_IT");
        append(tr("Fulah"), "ff_SN");
        append(tr("Ga"), "gaa_GH");
        append(tr("Galician"), "gl_ES");
        append(tr("Ganda"), "lg_UG");
        append(tr("Georgian"), "ka_GE");
        append(tr("German"), "de_DE");
        append(tr("Greek"), "el_GR");
        append(tr("Greenlandic"), "kl_GL");
        append(tr("Gujarati"), "gu_IN");
        append(tr("Gusii"), "guz_KE");
        append(tr("Hausa"), "ha_GH");
        append(tr("Hawaiian"), "haw_US");
        append(tr("Hebrew"), "he_IL");
        append(tr("Hindi"), "hi_IN");
        append(tr("Hungarian"), "hu_HU");
        append(tr("Icelandic"), "is_IS");
        append(tr("Igbo"), "ig_NG");
        append(tr("Indonesian"), "id_ID");
        append(tr("Irish"), "ga_IE");
        append(tr("Italian"), "it_IT");
        append(tr("Japanese"), "ja_JP");
        append(tr("Jju"), "kaj_NG");
        append(tr("Kabuverdianu"), "kea_CV");
        append(tr("Kabyle"), "kab_DZ");
        append(tr("Kalenjin"), "kln_KE");
        append(tr("Kamba"), "kam_KE");
        append(tr("Kannada"), "kn_IN");
        append(tr("Kazakh"), "kk_KZ");
        append(tr("Kikuyu"), "ki_KE");
        append(tr("Kinyarwanda"), "rw_RW");
        append(tr("Kirghiz"), "ky_KG");
        append(tr("Konkani"), "kok_IN");
        append(tr("Korean"), "ko_KR");
        append(tr("Koyra Chiini"), "khq_ML");
        append(tr("Koyraboro Senni"), "ses_ML");
        append(tr("Kurdish"), "ku_IR");
        append(tr("Langi"), "lag_TZ");
        append(tr("Laothian"), "lo_LA");
        append(tr("Latvian"), "lv_LV");
        append(tr("Lingala"), "ln_CD");
        append(tr("Lithuanian"), "lt_LT");
        append(tr("Low German"), "nds_DE");
        append(tr("Luo"), "luo_KE");
        append(tr("Luyia"), "luy_KE");
        append(tr("Macedonian"), "mk_MK");
        append(tr("Machame"), "jmc_TZ");
        append(tr("Makonde"), "kde_TZ");
        append(tr("Malagasy"), "mg_MG");
        append(tr("Malay"), "ms_MY");
        append(tr("Malayalam"), "ml_IN");
        append(tr("Maltese"), "mt_MT");
        append(tr("Manx"), "gv_GB");
        append(tr("Marathi"), "mr_IN");
        append(tr("Masai"), "mas_KE");
        append(tr("Meru"), "mer_KE");
        append(tr("Mongolian"), "mn_MN");
        append(tr("Morisyen"), "mfe_MU");
        append(tr("Nama"), "naq_NA");
        append(tr("Nepali"), "ne_NP");
        append(tr("North Ndebele"), "nd_ZW");
        append(tr("Northern Sami"), "se_FI");
        append(tr("Northern Sotho"), "nso_ZA");
        append(tr("Norwegian"), "nb_NO");
        append(tr("Nyankole"), "nyn_UG");
        append(tr("Nynorsk"), "nn_NO");
        append(tr("Occitan"), "oc_FR");
        append(tr("Oriya"), "or_IN");
        append(tr("Pashto"), "ps_AF");
        append(tr("Persian"), "fa_IR");
        append(tr("Polish"), "pl_PL");
        append(tr("Portuguese"), "pt_PT");
        append(tr("Punjabi"), "pa_IN");
        append(tr("RhaetoRomance"), "rm_CH");
        append(tr("Romanian"), "ro_MD");
        append(tr("Rombo"), "rof_TZ");
        append(tr("Rundi"), "rn_BI");
        append(tr("Russian"), "ru_RU");
        append(tr("Rwa"), "rwk_TZ");
        append(tr("Saho"), "ssy_ER");
        append(tr("Samburu"), "saq_KE");
        append(tr("Sangho"), "sg_CF");
        append(tr("Sena"), "seh_MZ");
        append(tr("Serbian"), "sr_CS");
        append(tr("SerboCroatian"), "sh_CS");
        append(tr("Sesotho"), "st_LS");
        append(tr("Setswana"), "tn_ZA");
        append(tr("Shambala"), "ksb_TZ");
        append(tr("Shona"), "sn_ZW");
        append(tr("Sichuan Yi"), "ii_CN");
        append(tr("Sidamo"), "sid_ET");
        append(tr("Singhalese"), "si_LK");
        append(tr("Siswati"), "ss_ZA");
        append(tr("Slovak"), "sk_SK");
        append(tr("Slovenian"), "sl_SI");
        append(tr("Soga"), "xog_UG");
        append(tr("Somali"), "so_SO");
        append(tr("South Ndebele"), "nr_ZA");
        append(tr("Spanish"), "es_ES");
        append(tr("Swahili"), "sw_KE");
        append(tr("Swedish"), "sv_SE");
        append(tr("Swiss German"), "gsw_CH");
        append(tr("Tachelhit"), "shi_MA");
        append(tr("Tagalog"), "tl_PH");
        append(tr("Taita"), "dav_KE");
        append(tr("Tajik"), "tg_TJ");
        append(tr("Tamil"), "ta_IN");
        append(tr("Taroko"), "trv_TW");
        append(tr("Telugu"), "te_IN");
        append(tr("Teso"), "teo_KE");
        append(tr("Thai"), "th_TH");
        append(tr("Tibetan"), "bo_CN");
        append(tr("Tigre"), "tig_ER");
        append(tr("Tigrinya"), "ti_ER");
        append(tr("Tonga"), "to_TO");
        append(tr("Tsonga"), "ts_ZA");
        append(tr("Turkish"), "tr_TR");
        append(tr("Tyap"), "kcg_NG");
        append(tr("Ukrainian"), "uk_UA");
        append(tr("Urdu"), "ur_IN");
        append(tr("Uzbek"), "uz_UZ");
        append(tr("Venda"), "ve_ZA");
        append(tr("Vietnamese"), "vi_VN");
        append(tr("Vunjo"), "vun_TZ");
        append(tr("Walamo"), "wal_ET");
        append(tr("Welsh"), "cy_GB");
        append(tr("Xhosa"), "xh_ZA");
        append(tr("Yoruba"), "yo_NG");
        append(tr("Zulu"), "zu_ZA");
    }
};

#endif // LOCALEMODEL_H
