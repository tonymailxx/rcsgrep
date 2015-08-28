// vim:ts=3:sts=3:sw=3

#include "TypeDef.h"
#include "AxUtil.h"

namespace td {

   namespace cond {

      namespace service {
         static const char *NAMETABLE[] =
         {
            "NOT_EXIST",
            "IM",
         };

         const char *name(service::e value)
         {
            if ((NOT_EXIST <= value) && (END_OF_ENUM > value)) {
               return NAMETABLE[value];
            }

            return "UNKNOWN";
         }

         service::e categorize(const char *name)
         {
            if          (axStrCaseMatch("im", name)) {
               return service::IM;
            }

            return service::NOT_EXIST;
         }
      }

   }

}
