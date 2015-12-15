#include "stdafx.h"
#include "base_device_data_types_defs.h"

const string v_manufacturer_name::v_manufacturer_onvif_name		= "onvif";
const string v_manufacturer_name::v_manufacturer_psia_name		= "psia";
const string v_manufacturer_name::v_manufacturer_gb28181_name	= "28181";
const string v_manufacturer_name::v_manufacturer_hdcctv_name	= "cctv";
const string v_manufacturer_name::v_manufacturer_acti_name		= "acti";
const string v_manufacturer_name::v_manufacturer_aebell_name	= "aebell";
const string v_manufacturer_name::v_manufacturer_axis_name		= "axis";
const string v_manufacturer_name::v_manufacturer_bluestar_name	= "bluestar";
const string v_manufacturer_name::v_manufacturer_canon_name		= "canon";
const string v_manufacturer_name::v_manufacturer_dahua_name		= "dahua";
const string v_manufacturer_name::v_manufacturer_hbgk_name		= "hbgk";
const string v_manufacturer_name::v_manufacturer_hikvison_name	= "hikvision";
const string v_manufacturer_name::v_manufacturer_huanghe		= "huanghe";
const string v_manufacturer_name::v_manufacturer_infinova		= "infinova";
const string v_manufacturer_name::v_manufacturer_langchi		= "langchi";
const string v_manufacturer_name::v_manufacturer_pelco			= "pelco";
const string v_manufacturer_name::v_manufacturer_samsung		= "samsung";
const string v_manufacturer_name::v_manufacturer_tdwy_name		= "tdwy";

data_cache_ex<v_device_manufacturer_type, string> ALL_DEVICE_NAME::all_name;
data_cache_ex<v_device_manufacturer_type, string>* ALL_DEVICE_NAME::all_device_names()
{
	return &all_name;
}

int ALL_DEVICE_NAME::add_device_name(v_device_manufacturer_type type_id, string name)
{
	if (!all_name.find(type_id, name))
	{
		all_name.insert(type_id, name);
	}
	return all_name.size();
}

int ALL_DEVICE_NAME::insert_all()
{
	if (1 > all_name.size())
	{
		all_name.insert(V_MANUFACTURER_ACTI, v_manufacturer_name::v_manufacturer_acti_name);
		all_name.insert(V_MANUFACTURER_AEBELL, v_manufacturer_name::v_manufacturer_aebell_name);
		all_name.insert(V_MANUFACTURER_AXIS, v_manufacturer_name::v_manufacturer_axis_name);
		all_name.insert(V_MANUFACTURER_BLUESTAR, v_manufacturer_name::v_manufacturer_bluestar_name);
		all_name.insert(V_MANUFACTURER_CANON, v_manufacturer_name::v_manufacturer_canon_name);
		all_name.insert(V_MANUFACTURER_DAHUA, v_manufacturer_name::v_manufacturer_dahua_name);
		all_name.insert(V_MANUFACTURER_HBGK, v_manufacturer_name::v_manufacturer_hbgk_name);
		all_name.insert(V_MANUFACTURER_HIKVISION, v_manufacturer_name::v_manufacturer_hikvison_name);
		all_name.insert(V_MANUFACTURER_HUANGHE, v_manufacturer_name::v_manufacturer_huanghe);
		all_name.insert(V_MANUFACTURER_INFINOVA, v_manufacturer_name::v_manufacturer_infinova);
		all_name.insert(V_MANUFACTURER_LANGCHI, v_manufacturer_name::v_manufacturer_langchi);
		all_name.insert(V_MANUFACTURER_PELCO, v_manufacturer_name::v_manufacturer_pelco);
		all_name.insert(V_MANUFACTURER_SAMSUNG, v_manufacturer_name::v_manufacturer_samsung);
		all_name.insert(V_MANUFACTURER_TDWY, v_manufacturer_name::v_manufacturer_tdwy_name);
		all_name.insert(V_MANUFACTURER_ONVIF, v_manufacturer_name::v_manufacturer_onvif_name);
		all_name.insert(V_MANUFACTURER_PSIA, v_manufacturer_name::v_manufacturer_psia_name);
		all_name.insert(V_MANUFACTURER_GB28181, v_manufacturer_name::v_manufacturer_gb28181_name);
		all_name.insert(V_MANUFACTURER_HDCCTV, v_manufacturer_name::v_manufacturer_hdcctv_name);
	}
	return all_name.size();
}
