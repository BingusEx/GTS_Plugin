#pragma once

namespace GtsUI {
    
    class ImCategory {
        protected:
            
        //Fields
        std::string title = "Default";
        std::string description = "Default";
        bool visible = true;

        virtual ~ImCategory() = default;

        public:

        //Virtuals
        virtual void Draw() = 0;
        
        //Methods
        [[nodiscard]] const inline std::string& GetTitle(){
            return title;
        }

        [[nodiscard]] const inline bool IsVisible(){
            return visible;
        }

        inline void SetVisible(const bool a_visible){
            visible = a_visible;
        }

    };
}