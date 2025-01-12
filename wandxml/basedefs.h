#ifndef _WANDXML_BASE_BASEDEFS_H_
#define _WANDXML_BASE_BASEDEFS_H_

#if WANDXML_DYNAMIC_LINK
	#if defined(_MSC_VER)
		#define WANDXML_DLLEXPORT __declspec(dllexport)
		#define WANDXML_DLLIMPORT __declspec(dllimport)
	#elif defined(__GNUC__) || defined(__clang__)
		#define WANDXML_DLLEXPORT __attribute__((__visibility__("default")))
		#define WANDXML_DLLIMPORT __attribute__((__visibility__("default")))
	#endif
#else
	#define WANDXML_DLLEXPORT
	#define WANDXML_DLLIMPORT
#endif

#if defined(_MSC_VER)
	#define WANDXML_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
	#ifndef NDEBUG
		#define WANDXML_FORCEINLINE __attribute__((__always_inline__)) inline
	#else
		#define WANDXML_FORCEINLINE inline
	#endif
#endif

#if defined(_MSC_VER)
	#define WANDXML_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		apiModifier extern template class name<__VA_ARGS__>;
	#define WANDXML_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		apiModifier template class name<__VA_ARGS__>;
#elif defined(__GNUC__) || defined(__clang__)
	#define WANDXML_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		extern template class apiModifier name<__VA_ARGS__>;
	#define WANDXML_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...) \
		template class name<__VA_ARGS__>;
#else
	#define WANDXML_DECL_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...)
	#define WANDXML_DEF_EXPLICIT_INSTANTIATED_CLASS(apiModifier, name, ...)
#endif

#if IS_WANDXML_BASE_BUILDING
	#define WANDXML_API WANDXML_DLLEXPORT
#else
	#define WANDXML_API WANDXML_DLLIMPORT
#endif

#endif
