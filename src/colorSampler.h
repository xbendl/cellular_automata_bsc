#ifndef COLOR_SAMPLER_H_INCLUDED
#define COLOR_SAMPLER_H_INCLUDED

class ColorSampler
{
    public:
    /// Typ pro ulozeni informaci o mnozine barev
    typedef std::vector<QColor> ColorDefinitionVector;

    static ColorDefinitionVector sampler;   //< Vzornik barev

    static void AddColor(QColor color) { sampler.push_back(color); }
    static ColorDefinitionVector GetSampler() { return sampler; }
};

#endif // COLOR_SAMPLER_H_INCLUDED
