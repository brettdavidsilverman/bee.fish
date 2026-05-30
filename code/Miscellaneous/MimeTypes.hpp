#ifndef BEE_FISH_MISCELLANEOUS__MIME_TYPES
#define BEE_FISH_MISCELLANEOUS__MIME_TYPES


namespace BeeFishMiscellaneous 
{
    struct MimeType
    {
        string contentType;
        string cacheControl;
        bool index;
    };

    inline static string _noCacheControl =
        "no-store, max-age=0";

#if defined(DEBUG) || defined(DISABLE_CACHE)
    inline static string _defaultCacheControl =
        _noCacheControl;

#else
    inline static string _defaultCacheControl =
        "public, max-age=60";
#endif

    inline static std::map<string, MimeType>
    _mimeTypes{
        {
            ".txt",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".html",
            {
                "text/html; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },

        {
            ".xhtml",
            {
                "application/xhtml+xml; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".js",
            {
                "text/javascript; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".json",
            {
                "application/json; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            "Makefile",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".css",
            {
                "text/css; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".jpg",
            {
                "image/jpeg",
                "public, max-age=31536000, immutable",
                false
            }
        },
        {
            ".png",
            {
                "image/png",
                "public, max-age=31536000, immutable",
                false
            }
        },
        {
            ".gif",
            {
                "image/gif",
                "public, max-age=31536000, immutable",
                false
            }
        },
        {
            ".ico",
            {
                "image/x-icon",
                "public, max-age=31536000, immutable",
                false
            }
        },
        {
            ".h",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".hpp",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".cpp",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },

        {
            ".c",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".ino",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".bin",
            {
                "application/octet-stream",
                _defaultCacheControl,
                false
            }
        },
        {
            ".sh",
            {
                "text/plain; charset=utf-8",
                _defaultCacheControl,
                true
            }
        },
        {
            ".wav",
            {
                "audio/wav",
                _defaultCacheControl,
                false
            }
        }
    };
}

#endif