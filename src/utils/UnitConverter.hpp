#pragma once
#include "fmt/format.h"
#include "config/Config.hpp"

namespace Gts {

    // const inline double ScaleToHeight(/*RE::Actor* a_Actor*/){
    //     return Scale
    // }

    // const inline double ScaleToKG(/*RE::Actor* a_Actor*/){

    //     //TODO Fix.;
    //     const float Scale = 1.0f;
    //     const int ActorWeight = 100;
    //     const int SMT = 1; //Actor.HasMagicEffect(SmallMassiveThreat) ? 6 : 1;
    //     constexpr float BaseWeight = 55.0f; //KG at 0 weight
    //     return BaseWeight * ((1.0f + ActorWeight / 200.f) * std::pow(Scale, 3)) * SMT;
    // }

    //Return KG
    const inline double ActorWeightFormula(){
        return 3123213322.0;
    }

    const inline double ActorHeightFormula(){
        return 124.82;
    }

    //Metric -> Imperial

    const inline double KiloToPound(const double a_kg){
        return a_kg / 0.45359237;
    }

    const inline double MetersToFeet(const double a_meter){
        return a_meter * 3.28084;
    }
    

    //Metric -> Mammoth (Because funny)

    const inline double MetersToMammoth(const double a_meter){
        return a_meter / 3.85;
    }

    const inline double KiloToMammoth(const double a_kg){
        return a_kg / 11113.01;
    }



    // Formaters

    const inline std::string FormatMetricHeight(const double a_meter){

        if(a_meter < 1.0f)
            return fmt::format("{:.0f} cm",a_meter * 100.f);

        if(a_meter > 500.0f)
            return fmt::format("{:.2f} Km", a_meter / 1000.f);
            
        return fmt::format("{:.2f} m", a_meter);
    }

    const inline std::string FormatImperialHeight(const double a_feet) {
        // Get the integer part for feet.
        int feet = static_cast<int>(a_feet);
        // Get the fractional part and convert it to inches.
        double fraction = a_feet - feet;
        int inches = static_cast<int>(std::round(fraction * 12));

        // Handle the case where rounding makes inches equal to 12.
        if (inches == 12) {
            feet += 1;
            inches = 0;
        }

        if (a_feet < 1.0)
            return fmt::format("{}\"", inches);


        if (a_feet > 2000.0)
            return fmt::format("{:.2f} mi", a_feet / 5280);

        return fmt::format("{}'{}\"", feet, inches);
    }

    const inline std::string FormatMammothHeight(const double a_mammoth){
        return fmt::format("{:.2f} Mammoths", a_mammoth);
    }

    const inline std::string FormatMetricWeight(const double a_kg){
        if(a_kg < 1.0f)
            return fmt::format("{:.0f} g",a_kg * 100.f);

        if(a_kg > 100000.0f)
            return fmt::format("{:.2f} kt", a_kg / 1000000.f);

        if(a_kg > 10000.0f)
            return fmt::format("{:.2f} t", a_kg / 1000.f);


        return fmt::format("{:.2f} kg", a_kg);
    }

    const inline std::string FormatImperialWeight(const double a_lb){
        if(a_lb < 1.0)
            return fmt::format("{:.0f} oz", a_lb * 16.0);

        if(a_lb > 2000000.0)
            return fmt::format("{:.2f} kt", a_lb / 2000000.0);

        if(a_lb > 2000.0)
            return fmt::format("{:.2f} t", a_lb / 2000.0);

        return fmt::format("{:.2f} lb", a_lb);
    }

    const inline std::string FormatMammothWeight(const double a_mammoth){
        return fmt::format("{:.0f} Mammoths", a_mammoth);
    }

    const inline std::string GetWeight(/*RE::Actor* a_Actor*/){
        std::string displayUnits = Config::GetUI().sDisplayUnits;
        if (displayUnits == "kImperial") 
            return FormatImperialWeight(KiloToPound(ActorWeightFormula(/*a_Actor*/)));

        else if (displayUnits == "kMammoth") 
            return FormatMammothWeight(KiloToMammoth(ActorWeightFormula(/*a_Actor*/)));
        
        else
            return FormatMetricWeight(ActorWeightFormula(/*a_Actor*/));
    }

    const inline std::string GetHeight(/*RE::Actor* a_Actor*/){
        std::string displayUnits = Config::GetUI().sDisplayUnits;
        if (displayUnits == "kImperial") 
            return FormatImperialHeight(MetersToFeet(ActorHeightFormula(/*a_Actor*/)));

        else if (displayUnits == "kMammoth") 
            return FormatMammothHeight(MetersToMammoth(ActorHeightFormula(/*a_Actor*/)));
        
        else
            return FormatMetricHeight(ActorHeightFormula(/*a_Actor*/));
    }
}