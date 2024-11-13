#include <QShortcut>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLineEdit>
#include <QApplication>
#include <cmath>
#include <tuple>
#include <QMessageBox>
#include "triangleSolver.h"
#include "ui_mainwindow.h"
#include <vector>


MathHelper::MathHelper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MathHelper)
{
    ui->setupUi(this);
    //

    //
    QShortcut *resetShortcut= new QShortcut(QKeySequence("Ctrl+z"),this);
    resetShortcut->setContext(Qt::ApplicationShortcut);
    connect(resetShortcut,&QShortcut::activated,this,&MathHelper::clearAllLineEdits);

    QShortcut *solveShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(solveShortcut, &QShortcut::activated, this, &MathHelper::on_btnSolve_clicked);

    ui->more_info->setText("<a href='#' style='color: white;'>Info</a>");
    ui->more_info->setTextFormat(Qt::RichText);
    ui->more_info->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->more_info->setOpenExternalLinks(false);


    // Connect the label's linkActivated signal to the slot
    connect(ui->btnSolve, &QPushButton::clicked, this, &MathHelper::on_btnSolve_clicked);
    connect(ui->btnReset, &QPushButton::clicked, this, &MathHelper::on_btnReset_clicked);
    connect(ui->more_info, &QLabel::linkActivated, this, &MathHelper::on_more_info_linkActivated);
    lineEdits = {
        {ui->lineEdit_AB, ui->lineEdit_angleA, ui->lineEdit_AM,ui->lineEdit_BiA},
        {ui->lineEdit_BC, ui->lineEdit_angleB, ui->lineEdit_BM,ui->lineEdit_BiB},
        {ui->lineEdit_AC, ui->lineEdit_angleC, ui->lineEdit_CM,ui ->lineEdit_BiC},
        {ui->lineEdit_Ha,ui->lineEdit_Hb , ui->lineEdit_Hc,ui->lineEdit_Area}
    };
    //
    for (auto& row : lineEdits) {
        for (auto& lineEdit : row) {
            lineEdit->installEventFilter(this);
        }
    }
    //
}


MathHelper::~MathHelper()
{
    delete ui;
}
bool MathHelper::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right) {
            keyPressEvent(keyEvent);
            return true; // Event is handled, don't propagate it further
        }
    }
    return QMainWindow::eventFilter(obj, event);
}


std::pair<int, int> MathHelper::findFocusedLineEdit()
{
    QWidget* focusedWidget = QApplication::focusWidget();
    for (int y =0 ;y < lineEdits.size();y++){
        for(int x = 0 ; x < lineEdits[y].size();x++){
            if (lineEdits[y][x]==focusedWidget){
                return {y,x};
            }

        }
    }
    return {-1,-1};
}
void MathHelper::keyPressEvent(QKeyEvent* event){
    if(event->key()==Qt::Key_Up){
        moveUp();
    }
    else if (event->key()==Qt::Key_Down){
        moveDown();
    }
    else if (event->key()==Qt::Key_Left){
        moveLeft();
    }
    else if (event->key()==Qt::Key_Right){
        moveRight();
    }

    else{
        QMainWindow::keyPressEvent(event);
    }

}
void MathHelper::moveUp(){
    auto [y, x] = findFocusedLineEdit();
    if (y!=-1 && x!=-1){
        int newY = (y-1) % lineEdits.size();
        lineEdits[newY][x]->setFocus();

    }
}
void MathHelper::moveDown(){
    auto[y,x]= findFocusedLineEdit();
    if (y!=-1 && x!=-1){
        int newY = (y+1) % lineEdits.size();
        lineEdits[newY][x]->setFocus();
    }
}
void MathHelper::moveLeft(){
    auto[y,x]=findFocusedLineEdit();
    if (y!=-1 , x!=-1){
        int newX = (x - 1+ lineEdits[y].size()) %lineEdits[y].size();
        lineEdits[y][newX]->setFocus();
    }
}
void MathHelper::moveRight(){
    auto[y,x]=findFocusedLineEdit();
    if (y!=-1 , x!=-1){
        int newX = (x + 1 )  % lineEdits[y].size();
        lineEdits[y][newX]->setFocus();
    }
}
const double PI = 3.14159265358979323846;
double toRadians(double degree) {
    return degree * (PI / 180.0);
}

double toDegrees(double radian) {
    return radian * (180.0 / PI);
}



void MathHelper::on_btnSolve_clicked() {
    bool ok;
    QString errorMessage;

    // Lambda function to get default value if input is empty or invalid
    auto defaultval = [&](QLineEdit* lineEdit) -> double {
        QString text = lineEdit->text();
        if (text.isEmpty()) {
            return 0.0; // Return default value if empty
        }
        double value = text.toDouble(&ok);
        if (!ok) {
            errorMessage = "Please enter a valid number.";
            ui->lineEdit_Error->setText(errorMessage);
            return 0.0; // Return default value in case of invalid conversion
        }
        return value;
    };

    // Define the validation lambda function
    auto validateInput = [&](QLineEdit* lineEdit, const QString& fieldName) -> bool {
        QString text = lineEdit->text();
        if (text.isEmpty()) {
            lineEdit->setText("0");
            return true; // Consider empty input as valid and set to default value
        }
        double value = lineEdit->text().toDouble(&ok);
        if (!ok) {
            errorMessage = "Please enter a number for " + fieldName + ".";
            ui->lineEdit_Error->setText(errorMessage);
            return false; // Indicate invalid input
        }
        return true; // Indicate valid input
    };

    // Validate each input field
    if (!validateInput(ui->lineEdit_AB, "AB") ||
        !validateInput(ui->lineEdit_AC, "AC") ||
        !validateInput(ui->lineEdit_BC, "BC") ||
        !validateInput(ui->lineEdit_angleA, "angleA") ||
        !validateInput(ui->lineEdit_angleB, "angleB") ||
        !validateInput(ui->lineEdit_angleC, "angleC") ||
        !validateInput(ui->lineEdit_AM, "median_AM") ||
        !validateInput(ui->lineEdit_BM, "median_BM") ||
        !validateInput(ui->lineEdit_CM, "median_CM") ||
        !validateInput(ui->lineEdit_Area, "Area") ||
        !validateInput(ui->lineEdit_BiA,"BisectorA") ||
        !validateInput(ui->lineEdit_BiB,"BisectorB") ||
        !validateInput(ui->lineEdit_BiC,"BisectorC") ||
        !validateInput(ui->lineEdit_Ha,"HeightAH") ||
        !validateInput(ui->lineEdit_Hb,"HeightBH") ||
        !validateInput(ui->lineEdit_Hc,"HeightCH"))
    {
        return; // Exit early if there are validation errors
    }




    // Get values with default handling
    double AB = defaultval(ui->lineEdit_AB);
    double AC = defaultval(ui->lineEdit_AC);
    double BC = defaultval(ui->lineEdit_BC);
    double angleA = defaultval(ui->lineEdit_angleA);
    double angleB = defaultval(ui->lineEdit_angleB);
    double angleC = defaultval(ui->lineEdit_angleC);
    double median_AM = defaultval(ui->lineEdit_AM);
    double median_BM = defaultval(ui->lineEdit_BM);
    double median_CM = defaultval(ui->lineEdit_CM);
    double Area = defaultval(ui->lineEdit_Area);
    double BisectorA = defaultval(ui->lineEdit_BiA);
    double BisectorB = defaultval(ui->lineEdit_BiB);
    double BisectorC = defaultval(ui->lineEdit_BiC);
    double HeightAH  = defaultval(ui->lineEdit_Ha);
    double HeightBH = defaultval(ui->lineEdit_Hb);
    double HeightCH = defaultval(ui->lineEdit_Hc);

    calculateMissingValues(AB, AC, BC, angleA, angleB, angleC, median_AM, median_BM, median_CM, Area,BisectorA,BisectorB,BisectorC,HeightAH,HeightBH,HeightCH);
}
std::tuple<double, double, double> calculate_3Angles(double AB, double AC, double BC) {
    double angleA = toDegrees(acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC)));
    double angleB = toDegrees(acos((AB * AB + BC * BC - AC * AC) / (2 * AB * BC)));
    double angleC = 180.0 - angleA - angleB; // Calculate angleC as the remaining angle

    return std::make_tuple(angleA, angleB, angleC);
}
void MathHelper::calculateMissingValues(double AB, double AC, double BC, double angleA, double angleB, double angleC, double median_AM, double median_BM, double median_CM, double Area,double BisectorA,double BisectorB,double BisectorC,double HeightAH,

double HeightBH,double HeightCH   ){

    double inRadius=0;
    double circumRadius=0  ;
    if (AB > 0 && BC > 0 && angleB > 0) {
        // Calculate AC using the Law of Cosines
        AC = sqrt(AB * AB + BC * BC - 2 * AB * BC * cos(toRadians(angleB)));

        // Calculate angleA using the Law of Sines
        angleA = toDegrees(asin(AB * sin(toRadians(angleB)) / AC));

        // Calculate angleC
        angleC = 180.0 - angleA - angleB;

        // Calculate Area
        Area = 0.5 * AB * BC * sin(toRadians(angleB));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;





    } else if (AC > 0 && BC > 0 && angleC > 0) {
        // Calculate AB using the Law of Cosines
        AB = sqrt(AC * AC + BC * BC - 2 * AC * BC * cos(toRadians(angleC)));

        // Calculate angleA using the Law of Sines
        angleA = toDegrees(asin(BC * sin(toRadians(angleC)) / AB));

        // Calculate angleB
        angleB = 180.0 - angleA - angleC;

        // Calculate Area
        Area = 0.5 * AC * BC * sin(toRadians(angleC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (AB > 0 && AC > 0 && angleA > 0) {
        // Calculate BC using the Law of Cosines
        BC = sqrt(AB * AB + AC * AC - 2 * AB * AC * cos(toRadians(angleA)));

        // Calculate angleB using the Law of Sines
        angleB = toDegrees(asin(AC * sin(toRadians(angleA)) / BC));

        // Calculate angleC
        angleC = 180.0 - angleA - angleB;

        // Calculate Area
        Area = 0.5 * AB * AC * sin(toRadians(angleA));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (AB > 0 && AC > 0 && Area > 0) {
        double Sin = Area / (0.5 * AB * AC);
        double final_sin = asin(Sin);
        angleA = toDegrees(final_sin);
        BC = sqrt(AB * AB + AC * AC - 2 * AB * AC * cos(toRadians(angleA)));

        // Calculate angleB using the Law of Sines
        angleB = toDegrees(asin(AC * sin(toRadians(angleA)) / BC));

        // Calculate angleC
        angleC = 180.0 - angleA - angleB;

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    } else if (AC > 0 && BC > 0 && Area > 0) {
        double Sin1 = Area / (0.5 * AC * BC);
        double final_sin1 = asin(Sin1);
        angleC = toDegrees(final_sin1);

        // Calculate angleA using the Law of Sines
        angleA = toDegrees(asin(BC * sin(toRadians(angleC)) / AC));

        // Calculate angleB
        angleB = 180.0 - angleA - angleC;

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (AB > 0 && BC > 0 && Area > 0) {
        double Sin2 = Area / (0.5 * AB * BC);
        double final_sin2 = asin(Sin2);
        angleB = toDegrees(final_sin2);

        angleA = toDegrees(asin(AB * sin(toRadians(angleB)) / BC));

        // Calculate angleC
        angleC = 180.0 - angleA - angleB;

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (angleA > 0 && angleB > 0) {
        angleC = 180 - angleA - angleB;
        if (AC > 0) {
            BC = AC * sin(toRadians(angleA)) / sin(toRadians(angleB));
        } else if (BC > 0) {
            AC = BC * sin(toRadians(angleB)) / sin(toRadians(angleA));
        }
        AB = sqrt(AC * AC + BC * BC - 2 * AC * BC * cos(toRadians(angleC)));

        Area = 0.5 * AB * AC * sin(toRadians(angleA));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (angleA > 0 && angleC > 0) {
        angleB = 180 - angleA - angleC;
        if (AB > 0) {
            BC = AB * sin(toRadians(angleA)) / sin(toRadians(angleC));
        } else if (BC > 0) {
            AB = BC * sin(toRadians(angleC)) / sin(toRadians(angleA));
        }
        AC = sqrt(AB * AB + BC * BC - 2 * AB * BC * cos(toRadians(angleB)));
        Area = 0.5 * AB * AC * sin(toRadians(angleA));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (angleB > 0 && angleC > 0) {
        angleA = 180 - angleB - angleC;
        if (AB > 0) {
            AC = AB * sin(toRadians(angleB)) / sin(toRadians(angleC));
        } else if (AC > 0) {
            AB = AC * sin(toRadians(angleC)) / sin(toRadians(angleB));
        }
        BC = sqrt(AB * AB + AC * AC - 2 * AB * AC * cos(toRadians(angleA)));
        Area = 0.5 * AB * AC * sin(toRadians(angleA));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);

        // Calculate the inradius
        double semiPerimeter = (AB + BC + AC) / 2;
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    } else if (AB > 0 && AC > 0 && BC > 0) {
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;


        // Calculate the inradius
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH=2* Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    }
    else if(median_AM>0 && AB>0 && AC>0){
        BC=sqrt(2*pow(AB,2)+2*pow(AC,2)-4*pow(median_AM,2));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));

        // Calculate the inradius
        inRadius = Area / semiPerimeter;

        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;

    }
    else if(median_AM>0 && AB>0 && BC>0){
        AC=sqrt((4*pow(median_AM,2)-2*pow(AB,2)+pow(BC,2))/2);
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));

        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        inRadius = Area / semiPerimeter;
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;



    }
    else if(median_AM>0 && AC>0 && BC>0){
        AB=sqrt((4*pow(median_AM,2)-2*pow(AC,2)+pow(BC,2)/2));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));
        inRadius = Area / semiPerimeter;
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    }
    else if (median_BM > 0 && AB > 0 && BC > 0) {
        AC = sqrt(2*pow(AB,2)+2*pow(BC,2)-4*pow(median_BM,2));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        inRadius = Area / semiPerimeter;
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    } else if (median_BM > 0 && AC > 0 && BC > 0) {
        AB = sqrt((4 * pow(median_BM, 2) - 2 * pow(BC, 2) + pow(AC, 2)) / 2);
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        inRadius = Area / semiPerimeter;
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;

    } else if (median_BM > 0 && AB > 0 && AC > 0) {
        BC = sqrt(2 * pow(AB, 2) + 2 * pow(AC, 2) - 4 * pow(median_BM, 2));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        inRadius = Area / semiPerimeter;
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;

    } else if (median_CM > 0 && AC > 0 && BC > 0) {
        AB = sqrt((2* pow(AC, 2) + 2 * pow(BC, 2) - 4*pow(median_CM, 2)));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        angleA =  toDegrees(acos((AB*AB + AC*AC - BC*BC) / (2*AB*AC)));
        angleB =  toDegrees(acos((AB*AB + BC*BC - AC*AC) / (2*AB*BC)));
        angleC =  toDegrees(acos((BC*BC + AC*AC - AB*AB) / (2*BC*AC)));
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        inRadius = Area / semiPerimeter;
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    } else if (median_CM > 0 && AB > 0 && BC > 0) {
        AC = sqrt((4 * pow(median_CM, 2) - 2 * pow(BC, 2) + pow(AB, 2)) / 2);
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;

        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        inRadius = Area / semiPerimeter;
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;


    } else if (median_CM > 0 && AB > 0 && AC > 0) {
        BC = sqrt(2 * pow(AB, 2) + 2 * pow(AC, 2) - 4 * pow(median_CM, 2));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;

        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;

    }
    //BIsector begin here
    else if(BisectorA >0 && AC>0 &&AB>0){
        BC = sqrt((AB*AC-pow(BisectorA,2)) *pow(AB+AC,2)/(AB*AC));

        // Corrected formula for BisectorB
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

        // Corrected formula for BisectorC
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));

        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

// Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;


// Calculate the inradius
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;

    }
    else if(BisectorA >0 && AB>0 && BC>0){

        double denominator = (4 * pow(BisectorA, 2) - pow(AB + BC, 2));
        AC = (AB * BC * (AB + BC)) / denominator;
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

        // Corrected formula for BisectorC
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;


        // Calculate the inradius
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;
    }
    else if (BisectorA > 0 && AC > 0 && BC > 0){
        double denominator = (4 * pow(BisectorA, 2) / pow(AC + BC, 2)) - 1;
        AB = (AC * BC * (AC + BC)) / denominator;
        BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

        // Corrected formula for BisectorC
        BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
        double semiPerimeter = (AB + AC + BC) / 2;
        Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

        // Calculate the circumradius
        circumRadius = (AB * BC * AC) / (4 * Area);
        auto [A, B, C] = calculate_3Angles(AB, AC, BC);
        angleA=A;
        angleB=B;
        angleC=C;


        // Calculate the inradius
        inRadius = Area / semiPerimeter;
        median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
        median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
        median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
        HeightAH= 2*Area/BC;
        HeightBH=2*Area/AC;
        HeightCH=2*Area/AB;
    }
    //Bisector B given
        else if (BisectorB > 0 && AB > 0 && BC > 0){
            AC = sqrt((AB*BC-pow(BisectorB,2)) *pow(AB+BC,2)/(AB*BC));
            BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            double semiPerimeter = (AB + AC + BC) / 2;
            Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);
            auto [A, B, C] = calculate_3Angles(AB, AC, BC);
            angleA=A;
            angleB=B;
            angleC=C;


            // Calculate the inradius
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;
    }
        else if (BisectorB > 0 && AC > 0 && BC > 0){
                double denominator = (4 * pow(BisectorB, 2) / pow(AC + BC, 2)) - 1;
                AB = (AC * BC * (AC + BC)) / denominator;
                BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));

                // Corrected formula for BisectorC
                BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
                double semiPerimeter = (AB + AC + BC) / 2;
                Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

                // Calculate the circumradius
                circumRadius = (AB * BC * AC) / (4 * Area);
                auto [A, B, C] = calculate_3Angles(AB, AC, BC);
                angleA=A;
                angleB=B;
                angleC=C;


                // Calculate the inradius
                inRadius = Area / semiPerimeter;
                median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
                median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
                median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
                HeightAH= 2*Area/BC;
                HeightBH=2*Area/AC;
                HeightCH=2*Area/AB;
        }
        //last second
        else if (BisectorB > 0 && AB > 0 && AC > 0){
            double denominator = (4 * pow(BisectorB, 2) / pow(AC + AB, 2)) - 1;
            BC = (AC * AB * (AC + AB)) / denominator;
            BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            double semiPerimeter = (AB + AC + BC) / 2;
            Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);
            auto [A, B, C] = calculate_3Angles(AB, AC, BC);
            angleA=A;
            angleB=B;
            angleC=C;


            // Calculate the inradius
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            HeightAH=2* Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;
        }
    //BisectorC given
        else if (BisectorC > 0 && BC > 0 && AC > 0){
            AB = sqrt((BC*AC-pow(BisectorC,2)) *pow(BC+AC,2)/(BC*AC));
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            double semiPerimeter = (AB + AC + BC) / 2;
            Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);
            auto [A, B, C] = calculate_3Angles(AB, AC, BC);
            angleA=A;
            angleB=B;
            angleC=C;


            // Calculate the inradius
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;
        }
    //second
        else if (BisectorC > 0 && AB > 0 && BC > 0){
            double denominator = (4 * pow(BisectorC, 2) / pow(AB + BC, 2)) - 1;
            AC = (AB * BC * (AB + BC)) / denominator;
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            double semiPerimeter = (AB + AC + BC) / 2;
            Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);
            auto [A, B, C] = calculate_3Angles(AB, AC, BC);
            angleA=A;
            angleB=B;
            angleC=C;


            // Calculate the inradius
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;
        }
    //third
        else if (BisectorC > 0 && AC > 0 && AB > 0){
            double denominator = (4 * pow(BisectorC, 2) / pow(AC + AB, 2)) - 1;
            BC = (AC * AB * (AC + AB)) / denominator;
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            double semiPerimeter = (AB + AC + BC) / 2;
            Area = sqrt(semiPerimeter * (semiPerimeter - AB) * (semiPerimeter - AC) * (semiPerimeter - BC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);
            auto [A, B, C] = calculate_3Angles(AB, AC, BC);
            angleA=A;
            angleB=B;
            angleC=C;


            // Calculate the inradius
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            HeightAH=2* Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;
        }
        //knowing 1bisector 1side 1 angle find another
        //angleA handle
        else if(angleA >0 && AC>0 &&BisectorA>0){
            AB=(-BisectorA*AC)/(BisectorA-2*AC*cos(toRadians(angleA/2)));
            // Calculate BC using the Law of Cosines
            BC = sqrt(AB * AB + AC * AC - 2 * AB * AC * cos(toRadians(angleA)));

            // Calculate angleB using the Law of Sines
            angleB = toDegrees(asin(AC * sin(toRadians(angleA)) / BC));

            // Calculate angleC
            angleC = 180.0 - angleA - angleB;

            // Calculate Area
            Area = 0.5 * AB * AC * sin(toRadians(angleA));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;

        }
        //second
        else if (angleA>0 && AB>0 && BisectorA>0){
            AC=(-BisectorA*AB)/(BisectorA-2*AB*cos(toRadians(angleA/2)));
            // Calculate BC using the Law of Cosines
            BC = sqrt(AB * AB + AC * AC - 2 * AB * AC * cos(toRadians(angleA)));

            // Calculate angleB using the Law of Sines
            angleB = toDegrees(asin(AC * sin(toRadians(angleA)) / BC));

            // Calculate angleC
            angleC = 180.0 - angleA - angleB;

            // Calculate Area
            Area = 0.5 * AB * AC * sin(toRadians(angleA));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;


        }
        //handle angleB
        else if(angleB>0 && BC>0 && BisectorB>0){
            AB=(-BisectorB*BC)/(BisectorB-2*BC*cos(toRadians(angleB/2)));
            // Calculate BC using the Law of Cosines
            AC = sqrt(AB * AB + BC * BC - 2 * AB * BC * cos(toRadians(angleB)));

            // Calculate angleA using the Law of Sines
            angleA = toDegrees(asin(AB * sin(toRadians(angleB)) / AC));

            // Calculate angleC
            angleC = 180.0 - angleA - angleB;

            // Calculate Area
            Area = 0.5 * AB * BC * sin(toRadians(angleB));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;

        }
        else if(angleB>0 && AB>0 && BisectorB>0){
            BC=(-BisectorB*AB)/(BisectorB-2*AB*cos(toRadians(angleB/2)));
            // Calculate BC using the Law of Cosines
            AC = sqrt(AB * AB + BC * BC - 2 * AB * BC * cos(toRadians(angleB)));

            // Calculate angleA using the Law of Sines
            angleA = toDegrees(asin(AB * sin(toRadians(angleB)) / AC));

            // Calculate angleC
            angleC = 180.0 - angleA - angleB;

            // Calculate Area
            Area = 0.5 * AB * BC * sin(toRadians(angleB));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorA = sqrt(AB * AC * (1 - pow(BC, 2) / pow(AB + AC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;

        }
        //handle angleC
        else if(angleC>0 && AC>0 && BisectorC>0){
            BC=(-BisectorC*AC)/(BisectorC-2*AC*cos(toRadians(angleC/2)));
            AB = sqrt(AC * AC + BC * BC - 2 * AC * BC * cos(toRadians(angleC)));

            // Calculate angleA using the Law of Sines
            angleA = toDegrees(asin(BC * sin(toRadians(angleC)) / AB));

            // Calculate angleB
            angleB = 180.0 - angleA - angleC;

            // Calculate Area
            Area = 0.5 * AC * BC * sin(toRadians(angleC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH= 2*Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;

        }
        else if(angleC>0 && BC>0 && BisectorC>0){
            AC=(-BisectorC*BC)/(BisectorC-2*BC*cos(toRadians(angleC/2)));
            AB = sqrt(AC * AC + BC * BC - 2 * AC * BC * cos(toRadians(angleC)));

            // Calculate angleA using the Law of Sines
            angleA = toDegrees(asin(BC * sin(toRadians(angleC)) / AB));

            // Calculate angleB
            angleB = 180.0 - angleA - angleC;

            // Calculate Area
            Area = 0.5 * AC * BC * sin(toRadians(angleC));

            // Calculate the circumradius
            circumRadius = (AB * BC * AC) / (4 * Area);

            // Calculate the inradius
            double semiPerimeter = (AB + BC + AC) / 2;
            inRadius = Area / semiPerimeter;
            median_AM=0.5*sqrt(2*pow(AB,2)+2*pow(AC,2)-pow(BC,2));
            median_BM=0.5*sqrt(2*pow(AB,2)+2*pow(BC,2)-pow(AC,2));
            median_CM=0.5*sqrt(2*pow(BC,2)+2*pow(AC,2)-pow(AB,2));
            BisectorB = sqrt(AB * BC * (1 - pow(AC, 2) / pow(AB + BC, 2)));

            // Corrected formula for BisectorC
            BisectorC = sqrt(AC * BC * (1 - pow(AB, 2) / pow(AC + BC, 2)));
            HeightAH=2* Area/BC;
            HeightBH=2*Area/AC;
            HeightCH=2*Area/AB;


        }









    // Output the calculated values
    ui->lineEdit_AB_result->setText(QString::number(AB));
    ui->lineEdit_AC_result->setText(QString::number(AC));
    ui->lineEdit_BC_result->setText(QString::number(BC));
    ui->lineEdit_angleA_result->setText(QString::number(angleA));
    ui->lineEdit_angleB_result->setText(QString::number(angleB));
    ui->lineEdit_angleC_result->setText(QString::number(angleC));
    ui->lineEdit_AM_result->setText(QString::number(median_AM));
    ui->lineEdit_BM_result->setText(QString::number(median_BM));
    ui->lineEdit_CM_result->setText(QString::number(median_CM));
    ui->lineEdit_Area_result->setText(QString::number(Area));
    ui->lineEdit_inRadius_result->setText(QString::number(inRadius));
    ui->lineEdit_circumRadius_result->setText(QString::number(circumRadius));
    ui->lineEdit_BiA_result->setText(QString::number(BisectorA));
    ui->lineEdit_BiB_result->setText(QString::number(BisectorB));
    ui->lineEdit_BiC_result->setText(QString::number(BisectorC));
    ui->lineEdit_Ha_result->setText(QString::number(HeightAH));
    ui->lineEdit_Hb_result->setText(QString::number(HeightBH));
    ui->lineEdit_Hc_result->setText(QString::number(HeightCH));


    this->angleA = angleA;
    this->angleB = angleB;
    this->angleC = angleC;

}

void MathHelper::convertAngle() {
    double angleA_rad = toRadians(angleA);
    double angleB_rad = toRadians(angleB);
    double angleC_rad = toRadians(angleC);

    double sinA = std::sin(angleA_rad);
    double cosA = std::cos(angleA_rad);
    double tanA = std::tan(angleA_rad);
    double cotA = (tanA != 0) ? 1.0 / tanA : std::numeric_limits<double>::infinity();

    double sinB = std::sin(angleB_rad);
    double cosB = std::cos(angleB_rad);
    double tanB = std::tan(angleB_rad);
    double cotB = (tanB != 0) ? 1.0 / tanB : std::numeric_limits<double>::infinity();

    double sinC = std::sin(angleC_rad);
    double cosC = std::cos(angleC_rad);
    double tanC = std::tan(angleC_rad);
    double cotC = (tanC != 0) ? 1.0 / tanC : std::numeric_limits<double>::infinity();

    // Handle special cases where angle = 90 degrees
    if (angleA == 90) {
        cosA = 0;
        tanA = std::numeric_limits<double>::infinity();
        cotA = 0;
    }
    if (angleB == 90) {
        cosB = 0;
        tanB = std::numeric_limits<double>::infinity();
        cotB = 0;
    }
    if (angleC == 90) {
        cosC = 0;
        tanC = std::numeric_limits<double>::infinity();
        cotC = 0;
    }

    // Create a message box to display the results
    QString results = QString(
                          "Angle A:\n"
                          "sin(A) = %1\n"
                          "cos(A) = %2\n"
                          "tan(A) = %3\n"
                          "cot(A) = %4\n\n"
                          "Angle B:\n"
                          "sin(B) = %5\n"
                          "cos(B) = %6\n"
                          "tan(B) = %7\n"
                          "cot(B) = %8\n\n"
                          "Angle C:\n"
                          "sin(C) = %9\n"
                          "cos(C) = %10\n"
                          "tan(C) = %11\n"
                          "cot(C) = %12"
                          ).arg(sinA).arg(cosA).arg(tanA).arg(cotA)
                          .arg(sinB).arg(cosB).arg(tanB).arg(cotB)
                          .arg(sinC).arg(cosC).arg(tanC).arg(cotC);

    QMessageBox::information(this, "Trigonometric Results", results);
}
void MathHelper::on_btnReset_clicked(){
    clearAllLineEdits();
}
void MathHelper::clearAllLineEdits(){
    // Clear all input fields
    ui->lineEdit_AB->clear();
    ui->lineEdit_AC->clear();
    ui->lineEdit_BC->clear();
    ui->lineEdit_angleA->clear();
    ui->lineEdit_angleB->clear();
    ui->lineEdit_angleC->clear();
    ui->lineEdit_AM->clear();
    ui->lineEdit_BM->clear();
    ui->lineEdit_CM->clear();
    ui->lineEdit_Area->clear();
    ui->lineEdit_BiA->clear();
    ui->lineEdit_BiB->clear();
    ui->lineEdit_BiC->clear();
    ui->lineEdit_Ha->clear();
    ui->lineEdit_Hb->clear();
    ui->lineEdit_Hc->clear();


    // Clear the error message
    ui->lineEdit_Error->clear();
}
void MathHelper::on_more_info_linkActivated(){

    convertAngle();
}


