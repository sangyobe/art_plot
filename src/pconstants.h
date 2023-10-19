#ifndef PCONSTANTS_H
#define PCONSTANTS_H

#include <QtGui/QColor>

#ifndef Q_COMPILER_UNIFORM_INIT
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
namespace QColorConstants {
namespace Svg {
const Q_DECL_UNUSED QColor aliceblue                (0xf0, 0xf8, 0xff);
const Q_DECL_UNUSED QColor antiquewhite             (0xfa, 0xeb, 0xd7);
const Q_DECL_UNUSED QColor aqua                     (0x00, 0xff, 0xff);
const Q_DECL_UNUSED QColor aquamarine               (0x7f, 0xff, 0xd4);
const Q_DECL_UNUSED QColor azure                    (0xf0, 0xff, 0xff);
const Q_DECL_UNUSED QColor beige                    (0xf5, 0xf5, 0xdc);
const Q_DECL_UNUSED QColor bisque                   (0xff, 0xe4, 0xc4);
const Q_DECL_UNUSED QColor black                    (0x00, 0x00, 0x00);
const Q_DECL_UNUSED QColor blanchedalmond           (0xff, 0xeb, 0xcd);
const Q_DECL_UNUSED QColor blue                     (0x00, 0x00, 0xff);
const Q_DECL_UNUSED QColor blueviolet               (0x8a, 0x2b, 0xe2);
const Q_DECL_UNUSED QColor brown                    (0xa5, 0x2a, 0x2a);
const Q_DECL_UNUSED QColor burlywood                (0xde, 0xb8, 0x87);
const Q_DECL_UNUSED QColor cadetblue                (0x5f, 0x9e, 0xa0);
const Q_DECL_UNUSED QColor chartreuse               (0x7f, 0xff, 0x00);
const Q_DECL_UNUSED QColor chocolate                (0xd2, 0x69, 0x1e);
const Q_DECL_UNUSED QColor coral                    (0xff, 0x7f, 0x50);
const Q_DECL_UNUSED QColor cornflowerblue           (0x64, 0x95, 0xed);
const Q_DECL_UNUSED QColor cornsilk                 (0xff, 0xf8, 0xdc);
const Q_DECL_UNUSED QColor crimson                  (0xdc, 0x14, 0x3c);
const Q_DECL_UNUSED QColor cyan                     (0x00, 0xff, 0xff);
const Q_DECL_UNUSED QColor darkblue                 (0x00, 0x00, 0x8b);
const Q_DECL_UNUSED QColor darkcyan                 (0x00, 0x8b, 0x8b);
const Q_DECL_UNUSED QColor darkgoldenrod            (0xb8, 0x86, 0x0b);
const Q_DECL_UNUSED QColor darkgray                 (0xa9, 0xa9, 0xa9);
const Q_DECL_UNUSED QColor darkgreen                (0x00, 0x64, 0x00);
const Q_DECL_UNUSED QColor darkgrey                 (0xa9, 0xa9, 0xa9);
const Q_DECL_UNUSED QColor darkkhaki                (0xbd, 0xb7, 0x6b);
const Q_DECL_UNUSED QColor darkmagenta              (0x8b, 0x00, 0x8b);
const Q_DECL_UNUSED QColor darkolivegreen           (0x55, 0x6b, 0x2f);
const Q_DECL_UNUSED QColor darkorange               (0xff, 0x8c, 0x00);
const Q_DECL_UNUSED QColor darkorchid               (0x99, 0x32, 0xcc);
const Q_DECL_UNUSED QColor darkred                  (0x8b, 0x00, 0x00);
const Q_DECL_UNUSED QColor darksalmon               (0xe9, 0x96, 0x7a);
const Q_DECL_UNUSED QColor darkseagreen             (0x8f, 0xbc, 0x8f);
const Q_DECL_UNUSED QColor darkslateblue            (0x48, 0x3d, 0x8b);
const Q_DECL_UNUSED QColor darkslategray            (0x2f, 0x4f, 0x4f);
const Q_DECL_UNUSED QColor darkslategrey            (0x2f, 0x4f, 0x4f);
const Q_DECL_UNUSED QColor darkturquoise            (0x00, 0xce, 0xd1);
const Q_DECL_UNUSED QColor darkviolet               (0x94, 0x00, 0xd3);
const Q_DECL_UNUSED QColor deeppink                 (0xff, 0x14, 0x93);
const Q_DECL_UNUSED QColor deepskyblue              (0x00, 0xbf, 0xff);
const Q_DECL_UNUSED QColor dimgray                  (0x69, 0x69, 0x69);
const Q_DECL_UNUSED QColor dimgrey                  (0x69, 0x69, 0x69);
const Q_DECL_UNUSED QColor dodgerblue               (0x1e, 0x90, 0xff);
const Q_DECL_UNUSED QColor firebrick                (0xb2, 0x22, 0x22);
const Q_DECL_UNUSED QColor floralwhite              (0xff, 0xfa, 0xf0);
const Q_DECL_UNUSED QColor forestgreen              (0x22, 0x8b, 0x22);
const Q_DECL_UNUSED QColor fuchsia                  (0xff, 0x00, 0xff);
const Q_DECL_UNUSED QColor gainsboro                (0xdc, 0xdc, 0xdc);
const Q_DECL_UNUSED QColor ghostwhite               (0xf8, 0xf8, 0xff);
const Q_DECL_UNUSED QColor gold                     (0xff, 0xd7, 0x00);
const Q_DECL_UNUSED QColor goldenrod                (0xda, 0xa5, 0x20);
const Q_DECL_UNUSED QColor gray                     (0x80, 0x80, 0x80);
const Q_DECL_UNUSED QColor green                    (0x00, 0x80, 0x00);
const Q_DECL_UNUSED QColor greenyellow              (0xad, 0xff, 0x2f);
const Q_DECL_UNUSED QColor grey                     (0x80, 0x80, 0x80);
const Q_DECL_UNUSED QColor honeydew                 (0xf0, 0xff, 0xf0);
const Q_DECL_UNUSED QColor hotpink                  (0xff, 0x69, 0xb4);
const Q_DECL_UNUSED QColor indianred                (0xcd, 0x5c, 0x5c);
const Q_DECL_UNUSED QColor indigo                   (0x4b, 0x00, 0x82);
const Q_DECL_UNUSED QColor ivory                    (0xff, 0xff, 0xf0);
const Q_DECL_UNUSED QColor khaki                    (0xf0, 0xe6, 0x8c);
const Q_DECL_UNUSED QColor lavender                 (0xe6, 0xe6, 0xfa);
const Q_DECL_UNUSED QColor lavenderblush            (0xff, 0xf0, 0xf5);
const Q_DECL_UNUSED QColor lawngreen                (0x7c, 0xfc, 0x00);
const Q_DECL_UNUSED QColor lemonchiffon             (0xff, 0xfa, 0xcd);
const Q_DECL_UNUSED QColor lightblue                (0xad, 0xd8, 0xe6);
const Q_DECL_UNUSED QColor lightcoral               (0xf0, 0x80, 0x80);
const Q_DECL_UNUSED QColor lightcyan                (0xe0, 0xff, 0xff);
const Q_DECL_UNUSED QColor lightgoldenrodyellow     (0xfa, 0xfa, 0xd2);
const Q_DECL_UNUSED QColor lightgray                (0xd3, 0xd3, 0xd3);
const Q_DECL_UNUSED QColor lightgreen               (0x90, 0xee, 0x90);
const Q_DECL_UNUSED QColor lightgrey                (0xd3, 0xd3, 0xd3);
const Q_DECL_UNUSED QColor lightpink                (0xff, 0xb6, 0xc1);
const Q_DECL_UNUSED QColor lightsalmon              (0xff, 0xa0, 0x7a);
const Q_DECL_UNUSED QColor lightseagreen            (0x20, 0xb2, 0xaa);
const Q_DECL_UNUSED QColor lightskyblue             (0x87, 0xce, 0xfa);
const Q_DECL_UNUSED QColor lightslategray           (0x77, 0x88, 0x99);
const Q_DECL_UNUSED QColor lightslategrey           (0x77, 0x88, 0x99);
const Q_DECL_UNUSED QColor lightsteelblue           (0xb0, 0xc4, 0xde);
const Q_DECL_UNUSED QColor lightyellow              (0xff, 0xff, 0xe0);
const Q_DECL_UNUSED QColor lime                     (0x00, 0xff, 0x00);
const Q_DECL_UNUSED QColor limegreen                (0x32, 0xcd, 0x32);
const Q_DECL_UNUSED QColor linen                    (0xfa, 0xf0, 0xe6);
const Q_DECL_UNUSED QColor magenta                  (0xff, 0x00, 0xff);
const Q_DECL_UNUSED QColor maroon                   (0x80, 0x00, 0x00);
const Q_DECL_UNUSED QColor mediumaquamarine         (0x66, 0xcd, 0xaa);
const Q_DECL_UNUSED QColor mediumblue               (0x00, 0x00, 0xcd);
const Q_DECL_UNUSED QColor mediumorchid             (0xba, 0x55, 0xd3);
const Q_DECL_UNUSED QColor mediumpurple             (0x93, 0x70, 0xdb);
const Q_DECL_UNUSED QColor mediumseagreen           (0x3c, 0xb3, 0x71);
const Q_DECL_UNUSED QColor mediumslateblue          (0x7b, 0x68, 0xee);
const Q_DECL_UNUSED QColor mediumspringgreen        (0x00, 0xfa, 0x9a);
const Q_DECL_UNUSED QColor mediumturquoise          (0x48, 0xd1, 0xcc);
const Q_DECL_UNUSED QColor mediumvioletred          (0xc7, 0x15, 0x85);
const Q_DECL_UNUSED QColor midnightblue             (0x19, 0x19, 0x70);
const Q_DECL_UNUSED QColor mintcream                (0xf5, 0xff, 0xfa);
const Q_DECL_UNUSED QColor mistyrose                (0xff, 0xe4, 0xe1);
const Q_DECL_UNUSED QColor moccasin                 (0xff, 0xe4, 0xb5);
const Q_DECL_UNUSED QColor navajowhite              (0xff, 0xde, 0xad);
const Q_DECL_UNUSED QColor navy                     (0x00, 0x00, 0x80);
const Q_DECL_UNUSED QColor oldlace                  (0xfd, 0xf5, 0xe6);
const Q_DECL_UNUSED QColor olive                    (0x80, 0x80, 0x00);
const Q_DECL_UNUSED QColor olivedrab                (0x6b, 0x8e, 0x23);
const Q_DECL_UNUSED QColor orange                   (0xff, 0xa5, 0x00);
const Q_DECL_UNUSED QColor orangered                (0xff, 0x45, 0x00);
const Q_DECL_UNUSED QColor orchid                   (0xda, 0x70, 0xd6);
const Q_DECL_UNUSED QColor palegoldenrod            (0xee, 0xe8, 0xaa);
const Q_DECL_UNUSED QColor palegreen                (0x98, 0xfb, 0x98);
const Q_DECL_UNUSED QColor paleturquoise            (0xaf, 0xee, 0xee);
const Q_DECL_UNUSED QColor palevioletred            (0xdb, 0x70, 0x93);
const Q_DECL_UNUSED QColor papayawhip               (0xff, 0xef, 0xd5);
const Q_DECL_UNUSED QColor peachpuff                (0xff, 0xda, 0xb9);
const Q_DECL_UNUSED QColor peru                     (0xcd, 0x85, 0x3f);
const Q_DECL_UNUSED QColor pink                     (0xff, 0xc0, 0xcb);
const Q_DECL_UNUSED QColor plum                     (0xdd, 0xa0, 0xdd);
const Q_DECL_UNUSED QColor powderblue               (0xb0, 0xe0, 0xe6);
const Q_DECL_UNUSED QColor purple                   (0x80, 0x00, 0x80);
const Q_DECL_UNUSED QColor red                      (0xff, 0x00, 0x00);
const Q_DECL_UNUSED QColor rosybrown                (0xbc, 0x8f, 0x8f);
const Q_DECL_UNUSED QColor royalblue                (0x41, 0x69, 0xe1);
const Q_DECL_UNUSED QColor saddlebrown              (0x8b, 0x45, 0x13);
const Q_DECL_UNUSED QColor salmon                   (0xfa, 0x80, 0x72);
const Q_DECL_UNUSED QColor sandybrown               (0xf4, 0xa4, 0x60);
const Q_DECL_UNUSED QColor seagreen                 (0x2e, 0x8b, 0x57);
const Q_DECL_UNUSED QColor seashell                 (0xff, 0xf5, 0xee);
const Q_DECL_UNUSED QColor sienna                   (0xa0, 0x52, 0x2d);
const Q_DECL_UNUSED QColor silver                   (0xc0, 0xc0, 0xc0);
const Q_DECL_UNUSED QColor skyblue                  (0x87, 0xce, 0xeb);
const Q_DECL_UNUSED QColor slateblue                (0x6a, 0x5a, 0xcd);
const Q_DECL_UNUSED QColor slategray                (0x70, 0x80, 0x90);
const Q_DECL_UNUSED QColor slategrey                (0x70, 0x80, 0x90);
const Q_DECL_UNUSED QColor snow                     (0xff, 0xfa, 0xfa);
const Q_DECL_UNUSED QColor springgreen              (0x00, 0xff, 0x7f);
const Q_DECL_UNUSED QColor steelblue                (0x46, 0x82, 0xb4);
const Q_DECL_UNUSED QColor tan                      (0xd2, 0xb4, 0x8c);
const Q_DECL_UNUSED QColor teal                     (0x00, 0x80, 0x80);
const Q_DECL_UNUSED QColor thistle                  (0xd8, 0xbf, 0xd8);
const Q_DECL_UNUSED QColor tomato                   (0xff, 0x63, 0x47);
const Q_DECL_UNUSED QColor turquoise                (0x40, 0xe0, 0xd0);
const Q_DECL_UNUSED QColor violet                   (0xee, 0x82, 0xee);
const Q_DECL_UNUSED QColor wheat                    (0xf5, 0xde, 0xb3);
const Q_DECL_UNUSED QColor white                    (0xff, 0xff, 0xff);
const Q_DECL_UNUSED QColor whitesmoke               (0xf5, 0xf5, 0xf5);
const Q_DECL_UNUSED QColor yellow                   (0xff, 0xff, 0x00);
const Q_DECL_UNUSED QColor yellowgreen              (0x9a, 0xcd, 0x32);

} // namespace Svg
} // namespace QColorConstants
#endif

const QColor color_arr[] = {QColorConstants::Svg::orangered,
                            QColorConstants::Svg::lime,
                            QColorConstants::Svg::crimson,
                            QColorConstants::Svg::tan,
                            QColorConstants::Svg::coral,
                            QColorConstants::Svg::teal,
                            QColorConstants::Svg::goldenrod,
                            QColorConstants::Svg::green,
                            QColorConstants::Svg::magenta,
                            QColorConstants::Svg::maroon,
                            QColorConstants::Svg::deeppink,
                            QColorConstants::Svg::deepskyblue,
                            QColorConstants::Svg::forestgreen,
                            QColorConstants::Svg::fuchsia,
                            QColorConstants::Svg::olive,
                            QColorConstants::Svg::blueviolet,
                            QColorConstants::Svg::orchid,
                            QColorConstants::Svg::salmon,
                            QColorConstants::Svg::slategray,
                            QColorConstants::Svg::slateblue,
                            QColorConstants::Svg::darkblue,
                            QColorConstants::Svg::darkcyan,
                            QColorConstants::Svg::darkgoldenrod,
                            QColorConstants::Svg::darkgray,
                            QColorConstants::Svg::darkgreen,
                            QColorConstants::Svg::darkkhaki,
                            QColorConstants::Svg::darkmagenta,
                            QColorConstants::Svg::darkolivegreen,
                            QColorConstants::Svg::darkorange,
                            QColorConstants::Svg::darkorchid,
                            QColorConstants::Svg::darkred,
                            QColorConstants::Svg::darksalmon,
                            QColorConstants::Svg::darkseagreen,
                            QColorConstants::Svg::darkslateblue,
                            QColorConstants::Svg::darkturquoise,
                            QColorConstants::Svg::darkviolet,
                            QColorConstants::Svg::mediumaquamarine,
                            QColorConstants::Svg::mediumblue,
                            QColorConstants::Svg::mediumorchid,
                            QColorConstants::Svg::mediumpurple,
                            QColorConstants::Svg::mediumseagreen,
                            QColorConstants::Svg::mediumslateblue,
                            QColorConstants::Svg::mediumspringgreen,
                            QColorConstants::Svg::mediumturquoise,
                            QColorConstants::Svg::mediumvioletred,
                            QColorConstants::Svg::midnightblue,
                            QColorConstants::Svg::lightgreen,
                            QColorConstants::Svg::lightpink,
                            QColorConstants::Svg::lightsalmon,
                            QColorConstants::Svg::lightseagreen,
                            QColorConstants::Svg::lightskyblue,
                            QColorConstants::Svg::lightslategray,
                            QColorConstants::Svg::lightslategrey,
                            QColorConstants::Svg::lightsteelblue};

template <uint16_t line_number> inline const QColor &LineColor() {
    const uint16_t color_len = sizeof(color_arr) / sizeof(color_arr[0]);
    return color_arr[line_number % color_len];
}

inline const QColor& LineColor(int index)
{
    const uint16_t color_len = sizeof(color_arr) / sizeof(color_arr[0]);
    return color_arr[index % color_len];
}

enum LineScatterShape { ssNone       ///< no scatter symbols are drawn (e.g. in QCPGraph, data only represented with lines)
                    ,ssDot       ///< \enumimage{ssDot.png} a single pixel (use \ref ssDisc or \ref ssCircle if you want a round shape with a certain radius)
                    ,ssCross     ///< \enumimage{ssCross.png} a cross
                    ,ssPlus      ///< \enumimage{ssPlus.png} a plus
                    ,ssCircle    ///< \enumimage{ssCircle.png} a circle
                    ,ssDisc      ///< \enumimage{ssDisc.png} a circle which is filled with the pen's color (not the brush as with ssCircle)
                    ,ssSquare    ///< \enumimage{ssSquare.png} a square
                    ,ssDiamond   ///< \enumimage{ssDiamond.png} a diamond
                    ,ssStar      ///< \enumimage{ssStar.png} a star with eight arms, i.e. a combination of cross and plus
                    ,ssTriangle  ///< \enumimage{ssTriangle.png} an equilateral triangle, standing on baseline
                    ,ssTriangleInverted ///< \enumimage{ssTriangleInverted.png} an equilateral triangle, standing on corner
                    ,ssCrossSquare      ///< \enumimage{ssCrossSquare.png} a square with a cross inside
                    ,ssPlusSquare       ///< \enumimage{ssPlusSquare.png} a square with a plus inside
                    ,ssCrossCircle      ///< \enumimage{ssCrossCircle.png} a circle with a cross inside
                    ,ssPlusCircle       ///< \enumimage{ssPlusCircle.png} a circle with a plus inside
                    ,ssPeace     ///< \enumimage{ssPeace.png} a circle, with one vertical and two downward diagonal lines
};

#endif // PCONSTANTS_H
