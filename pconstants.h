#ifndef PCONSTANTS_H
#define PCONSTANTS_H

#include <QtGui/QColor>


constexpr QColor color_arr[] = {
    QColorConstants::Svg::crimson,
    QColorConstants::Svg::coral,
    QColorConstants::Svg::gold,
    QColorConstants::Svg::greenyellow,
    QColorConstants::Svg::mediumseagreen,
    QColorConstants::Svg::lightskyblue,
    QColorConstants::Svg::mediumvioletred,
    QColorConstants::Svg::darkcyan,
    QColorConstants::Svg::mediumorchid,
    QColorConstants::Svg::mediumturquoise,
    QColorConstants::Svg::lightsteelblue,
    QColorConstants::Svg::lightpink,
    QColorConstants::Svg::orchid,
    QColorConstants::Svg::teal,
    QColorConstants::Svg::tan,
    QColorConstants::Svg::lime,
    QColorConstants::Svg::salmon,
    QColorConstants::Svg::slategray,
    QColorConstants::Svg::slateblue
};

template<uint16_t line_number>
inline const QColor& LineColor()
{
    const uint16_t color_len = sizeof(color_arr) / sizeof(color_arr[0]);
    return color_arr[line_number % color_len];
}

inline const QColor& LineColor(int index)
{
    const uint16_t color_len = sizeof(color_arr) / sizeof(color_arr[0]);
    return color_arr[index % color_len];
}


#endif // PCONSTANTS_H
