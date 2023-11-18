#ifndef BEE_FISH_DATABASE__DATABASE_HPP
#define BEE_FISH_DATABASE__DATABASE_HPP

#include "Config.hpp"

#include <sys/mman.h>

#include <string.h>
#include <map>
#include <mutex>
#include "../PowerEncoding/PowerEncoding.hpp"

#include "Version.hpp"
#include "File.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "DatabaseBase.hpp"
#include "Data.hpp"#include "Path.hpp"

namespace BeeFishDatabase {
   typedef
      Path<Database::Encoding>::Stack
      Stack;
}

#endif


