#ifndef BPLUS_TEMPLATES
#define BPLUS_TEMPLATES                                                        \
  size_t M, properKeyValue Key, properKeyValue T,                              \
      Indexor<Key, std::pair<Key, T>> Indexor,                                 \
      std::predicate<Key, Key> Compare, IsAllocator Allocator
#endif

#ifndef BPLUS_TEMPLATE_PARAMS
#define BPLUS_TEMPLATE_PARAMS M, Key, T, Indexor, Compare, Allocator
#endif

#ifndef NODE_TEMPLATES
#define NODE_TEMPLATES BPLUS_TEMPLATES, size_t MAX_CHILDS, size_t MAX_KEYS
#endif

#ifndef NODE_TEMPLATE_PARAMS
#define NODE_TEMPLATE_PARAMS BPLUS_TEMPLATE_PARAMS, MAX_CHILDS, MAX_KEYS
#endif
