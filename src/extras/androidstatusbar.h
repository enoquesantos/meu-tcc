#ifndef ANDROIDSTATUSBAR_H
#define ANDROIDSTATUSBAR_H

#include <QObject>
#include <QColor>

/**
 * @brief The StatusBar class
 * This class is a nutshell to Android System Status Bar handle and can be used to set
 * a custom color in runtime.The color can be a hexadecimal or RGB.
 * To more details of Android Status Bar check the follow link:
 * @link https://material.io/guidelines/layout/structure.html#structure-system-bars
 */
class AndroidStatusBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(bool available READ isAvailable CONSTANT)
public:
    /**
     * @brief StatusBar
     * The object conastruct
     * @param parent *QObject the parent object
     */
    explicit AndroidStatusBar(QObject *parent = nullptr);

    /**
     * @brief color
     * Get the current color from m_color
     * @return QColor
     */
    QColor color() const;

    /**
     * @brief setColor
     * Set a dynamic color to android status bar
     * @param color QColor a hexadecial string or color name like "blue", "red" or "green"
     */
    void setColor(const QColor &color);

    /**
     * @brief isAvailable
     * Return true if android version supports to change the system status bar color
     * otherwise return false
     * @return bool
     */
    bool isAvailable() const;

private:
    /**
     * @brief m_color
     * a instance of QColor
     */
    QColor m_color;
};

#endif // ANDROIDSTATUSBAR_H
