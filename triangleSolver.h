#ifndef TRIANGLESOLVER_H
#define TRIANGLESOLVER_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QEvent>
#include <QMap>
#include <QWidget>
#include <QLineEdit>
QT_BEGIN_NAMESPACE
namespace Ui {
class MathHelper;
}
QT_END_NAMESPACE

class MathHelper : public QMainWindow
{
    Q_OBJECT

public:
    MathHelper(QWidget *parent = nullptr);
    ~MathHelper();
protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_btnSolve_clicked();
    void on_btnReset_clicked();
    void on_more_info_linkActivated();
    void clearAllLineEdits();    // Slot to clear all QLineEdit widgets

private:
    Ui::MathHelper *ui;
    std::vector<std::vector<QLineEdit*>> lineEdits;
    std::pair<int, int> findFocusedLineEdit();
    bool eventFilter(QObject *obj, QEvent *event);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    double angleA;
    double angleB;
    double angleC;

    void calculateMissingValues(double AB, double AC, double BC, double angleA, double angleB, double angleC, double median_AM, double median_BM, double median_CM, double Area,double BisectorA, double BisectorB,double BisectorC,double HeightAH,double HeightBH,double HeightCH);
    void convertAngle();
};
#endif // TRIANGLESOLVER_H
