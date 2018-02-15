#include "lrtastar.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPen>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setMargin(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    for(int r = 0 ; r < m.width() ; r++)
        for(int c = 0 ; c < m.width() ; c++)
            ui->gridLayout->addWidget(new Ui::SquareWidget(&m, &hitcount, r, c), r, c);
    //ai = new OnlineDFS(&m);
    ai = new LRTAStar(&m);
    previous_state = m._at();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_pushButton_clicked()));
    timer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Ui::SquareWidget::SquareWidget(Map *m, std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> *hitcount, int row, int col) {
    this->m = m;
    this->hitcount = hitcount;
    this->row = row;
    this->col = col;
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
}

void Ui::SquareWidget::redraw_if(int current_location) {
    if(row * m->width() + col == current_location) {
        this->update();
    }
}

void Ui::SquareWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QRect r = this->rect();
    int a = (*hitcount)[{{row,col}}];
    if(a) a += 10;
    a = (a < 256 ? a : 255);
    QColor bg(255,0,0,a);
    QBrush br(bg);
    painter.setBrush(br);
    painter.fillRect(r, br);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    int move = m->moves(row, col);
    if(!(move & UP))
        painter.drawLine(r.topLeft(), r.topRight());
    if(!(move & DOWN))
        painter.drawLine(r.bottomLeft(), r.bottomRight());
    if(!(move & LEFT))
        painter.drawLine(r.topLeft(), r.bottomLeft());
    if(!(move & RIGHT))
        painter.drawLine(r.topRight(), r.bottomRight());

    QRect c = r;
    c.adjust(1,1,-2,-2);
    if(m->isgoal(row, col)) {
        painter.setBrush(QBrush(Qt::green));
        painter.drawEllipse(c);
    }
    if(m->_at(row, col)) {
        painter.setBrush(QBrush(Qt::red));
        painter.drawEllipse(c);
    }
}

void MainWindow::on_pushButton_clicked()
{
    DIRECTION action = ai->nextaction(m.available_actions());
    std::cout << "Action is " << (action ==STOP ? "STOP" : (action == UP ? "UP" : (action == DOWN ? "DOWN" : (action == LEFT ? "LEFT" : "RIGHT")))) << std::endl << std::flush;
    if(action == STOP) return;
    m.move(action);
    int r = m._at() / m.width();
    int c = m._at() % m.width();
    hitcount[{{r,c}}]++;
    std::cout << "MAP: Moved " << Debug::d(action) << " from " << previous_state << " to " << m._at() << std::endl << std::flush;
    QListIterator<QObject *> i(ui->centralWidget->children());
    while(i.hasNext()) {
        if(Ui::SquareWidget *sw = dynamic_cast<Ui::SquareWidget *>(i.next())) {
          sw->redraw_if(previous_state);
          sw->redraw_if(m._at());
        }
    }
    previous_state = m._at();
}
