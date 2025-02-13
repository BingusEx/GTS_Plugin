#pragma once

#include "ImCategory.hpp"

namespace GtsUI {

 class ImCategoryManager {

        private:

        //Singleton Stuff
        ImCategoryManager() = default;
        ImCategoryManager(const ImCategoryManager&) = delete;
        ImCategoryManager& operator=(const ImCategoryManager&) = delete;

        //Fields
        std::vector<std::shared_ptr<ImCategory>> categories;

        public:
        uint8_t activeIndex;
        ~ImCategoryManager() = default;
        
        [[nodiscard]] static inline ImCategoryManager& GetSingleton() {
            static ImCategoryManager instance;
            return instance;
        }

        [[nodiscard]] inline std::vector<std::shared_ptr<ImCategory>>& GetCategories(){
            return categories;
        }

        inline void AddCategory(std::shared_ptr<ImCategory> category){
            categories.push_back(std::move(category));
        }

        [[nodiscard]] float GetLongestCategory();

    };
}
