#pragma once

#include <model/Page.h>

#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QStack>

class WorkArea : public QLabel {
    Q_OBJECT

// constants
private:
    const double LabelWidgetSizeRatio = 0.035;
    const double LabelWidgetFontSizeRation = 0.0175;

private:
    QWidget *containerWidget;

public:
    explicit WorkArea(QWidget *parent = nullptr);
    ~WorkArea() override = default;

    void setContainerWidget(QWidget *widget);

    // zoom management
private:
    static constexpr int MinimumZoomLevelIndex = -15;
    static constexpr int MaximumZoomLevelIndex = 15;
    static constexpr int ZoomLevelCount = MaximumZoomLevelIndex - MinimumZoomLevelIndex + 1;
    static constexpr double ZoomLevels[ZoomLevelCount] = {
        0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95,
        1.00,
        1.05, 1.10, 1.15, 1.20, 1.25, 1.30, 1.35, 1.40, 1.45, 1.50, 1.60, 1.70, 1.80, 1.90, 2.00
    };

    static constexpr double zoomLevel(int index) {
        return ZoomLevels[index - MinimumZoomLevelIndex];
    }

    int currentZoomLevelIndex;
    double currentZoomLevel() const;
    void setZoomLevel(int newIndex, bool forced = false);
    void setPreferredZoomLevel();

public slots:
    void zoom(bool in);
    void zoomReset();

signals:
    void sizeChanged();

    // page and label management
private:
    PageOperator *op;
    QPixmap pic;

    bool suppressExternalSignal;

    QVector<QPushButton*> labelWidgets;
    QStack<QPushButton*> unusedLabelWidgets;
    QFont f;
    int currentLabelWidgetSize;

    static void markLabelWidgetAsSelected(QPushButton *widget);
    static void markLabelWidgetAsUnselected(QPushButton *widget);
    QPushButton* addLabelWidget(const QPoint &position);

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;

public:
    void setPageOperator(PageOperator *op);

public slots:
    void onNewPage();

    // as WorkArea is the only component able to add label now
    // void onLabelAppended();
    void onLabelDeleted(QBitArray deleted);
    void onLabelSelectionUpdated(QBitArray selected);
};
