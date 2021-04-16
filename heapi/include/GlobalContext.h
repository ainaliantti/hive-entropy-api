#ifndef  GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <string>
#include <map>

/**
 * @brief Wrapper for a static map associating identifiers with managed objects.
 * 
 * @tparam C The class handled by the context.
 */
template<class C>
class GlobalContext{
    public:

        /**
         * @brief Inserts an object into the map.
         * 
         * @param id The identifier of the managed object.
         * @param o The object to manage.
         */
        static void registerObject(std::string id, C o);

        /**
         * @brief Retreives an object from the map.
         * 
         * @param id The identifier of the object to retreive.
         * @return C The retrieved object.
         */
        static C get(std::string id);
    private:
        static std::map<std::string,C> repository = std::map<std::string,C>();
};



template<class C>
void GlobalContext<C>::registerObject(std::string id, C o){
    GlobalContext::repository.insert(std::pair<std::string,C>(id,o));
}

template<class C>
C GlobalContext<C>::get(std::string id){
    return GlobalContext::repository.at(id);
}

#endif