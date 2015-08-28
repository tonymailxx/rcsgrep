// vim:ts=3:sts=3:sw=3

#ifndef RCSGREP_TYPEDEF_H__
#define RCSGREP_TYPEDEF_H__


namespace td {

   namespace cond {

      namespace service {
         typedef enum {
            NOT_EXIST = 0,
            IM,
            END_OF_ENUM
         } e;

         const char *name(service::e value);
         service::e categorize(const char *name);
      }

   }

}

#endif
