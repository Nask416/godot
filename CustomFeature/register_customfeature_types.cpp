#include "register_customfeature_types.h"
#include "global_plugin_settings.h"
#include "gds_atomic.h"



void register_customfeature_types()
{
#ifdef TOOLS_ENABLED
	GDREGISTER_CLASS(NasK::GlobalPluginSettings);
#endif

	GDREGISTER_CLASS(NasK::aint8);
	GDREGISTER_CLASS(NasK::auint8);
	GDREGISTER_CLASS(NasK::aint16);
	GDREGISTER_CLASS(NasK::auint16);
	GDREGISTER_CLASS(NasK::aint32);
	GDREGISTER_CLASS(NasK::auint32);
	GDREGISTER_CLASS(NasK::aint64);
	GDREGISTER_CLASS(NasK::auint64);
	GDREGISTER_CLASS(NasK::abool);
}

void unregister_customfeature_types()
{
}
