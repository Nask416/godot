#include "register_customfeature_types.h"




void register_customfeature_types()
{
#ifdef TOOLS_ENABLED
GDREGISTER_CLASS(NasK::GlobalPluginSettings);
#endif

}

void unregister_customfeature_types()
{
}
