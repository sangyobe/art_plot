#ifndef PCONSTANTS_H
#define PCONSTANTS_H

#include <QtGui/QColor>

#if (QT_VERSION_MAJOR >= 6 || (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 14))
const QColor color_arr[] = {
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
#else
const QColor color_arr[] = {
    QColor(Qt::red),
    QColor(Qt::green),
    QColor(Qt::blue),
    QColor(Qt::cyan),
    QColor(Qt::magenta),
    QColor(Qt::yellow),
    QColor(Qt::darkRed),
    QColor(Qt::darkGreen),
    QColor(Qt::darkBlue),
    QColor(Qt::darkCyan),
    QColor(Qt::darkMagenta),
    QColor(Qt::darkYellow),
    QColor(Qt::gray),
    QColor(Qt::darkGray),
    QColor(Qt::lightGray)
};
#endif

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
