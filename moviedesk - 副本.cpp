#include "moviedesk.h"
#include "ui_moviedesk.h"
#include <QMessageBox>
#include "midwindow.h"
#include "frmmessagebox.h"
movieDesk* pStaticMovieDesk=NULL;

bool pic_slave_or_master;
bool video_slave_or_master;

extern QFileInfo fileInfo_to_play;
extern QFileInfo fileInfo_to_show;
extern MidWindow* midwindow;
movieDesk::movieDesk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::movieDesk),cur_index(0)
{
    ui->setupUi(this);
    pStaticMovieDesk=this;

    connect(ui->returnButton,SIGNAL(clicked(bool)),SLOT(on_returnButton_clicked()));

    //将Tab设置为横向
    ui->tabWidget->setTabPosition(QTabWidget::West);
//    ui->tabWidget->setTabShape( QTabWidget::Rounded );
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle(90,60));
    ui->tabWidget->setWindowFlags(Qt::FramelessWindowHint);
    ui->tabWidget->clear();
    ui->tabWidget->setCornerWidget(ui->viceButton,Qt::TopRightCorner);
    #if 0
    ui->tabWidget->setStyleSheet("QTabWidget::pane{ \
            border-left: 1px solid #eeeeee;\
        }");
    #endif
    //为tabbar加入widget
    slave_picturewidget=new slave_PictureWidget(this);
    slave_videowidget=new slave_VideoWidget(this);
    videowidget=new videoWidget(this);
//    editwidget=new editWidget();
    fileDialog=new QFileDialog(this,Qt::SubWindow);
    picturewidget=new pictureWidget(this);
//    fileDialog->setUpdatesEnabled(true);

#if defined(Q_OS_LINUX)
    fileDialog->setDirectory("/");
#else
    fileDialog->setDirectory("E:");
#endif
    connect(fileDialog,SIGNAL(fileSelected(QString)),this,SLOT(open_file(QString)));

    ui->tabWidget->addTab(videowidget,tr("video"));
    ui->tabWidget->insertTab(1,picturewidget,tr("picture"));
    ui->tabWidget->insertTab(2,fileDialog,tr("find..."));

    connect(picturewidget,SIGNAL(hide_moviedesktop()),this,SLOT(on_hide_moviedesktop()));
    connect(picturewidget,SIGNAL(on_unhide_moviedesktop()),this,SLOT(on_unhide_moviedesktop()));

    connect(slave_picturewidget,SIGNAL(hide_moviedesktop()),this,SLOT(on_hide_moviedesktop()));
    connect(slave_picturewidget,SIGNAL(on_unhide_moviedesktop()),this,SLOT(on_unhide_moviedesktop()));

    connect(videowidget,SIGNAL(hide_moviedesktop()),this,SLOT(on_hide_moviedesktop()));
    connect(videowidget,SIGNAL(on_unhide_moviedesktop()),this,SLOT(on_unhide_moviedesktop()));

    connect(slave_videowidget,SIGNAL(hide_moviedesktop()),this,SLOT(on_hide_moviedesktop()));
    connect(slave_videowidget,SIGNAL(on_unhide_moviedesktop()),this,SLOT(on_unhide_moviedesktop()));

    connect(fileDialog,SIGNAL(finished(int)),this,SLOT(deal_fileDialog(int)));
    connect(fileDialog,SIGNAL(accepted()),this,SLOT(deal_fileDialog1()));

    connect(midwindow,SIGNAL(usb_insert()),this,SLOT(on_usb_mount()));
    connect(midwindow,SIGNAL(usb_delete()),this,SLOT(on_usb_umount()));
}

movieDesk::~movieDesk()
{
    delete ui;
}
void movieDesk::on_usb_mount()
{
    qDebug()<<"filedialog get the signal usb insert";
    frmMessageBox *msg=new frmMessageBox;
    msg->SetMessage(QString(tr("usb is insert!")),0);
    QTimer::singleShot(2000, msg, SLOT(close()));
//    msg->exec();
    fileDialog->update();
}
void movieDesk::on_usb_umount()
{
    qDebug()<<"filedialog get the signal usb delete";

    frmMessageBox *msg=new frmMessageBox;
    msg->SetMessage(QString(tr("usb is out!")),0);
    QTimer::singleShot(2000, msg, SLOT(close()));
//    msg->exec();
    fileDialog->update();
}

void movieDesk::deal_fileDialog(int a)
{
    qDebug()<<"cancel is done"<<a;
    fileDialog->show();
}
void movieDesk::deal_fileDialog1()
{
    qDebug()<<"accept is done";
    fileDialog->show();
}
void movieDesk::open_file(QString fileName)
{
//    QMessageBox::information(NULL, tr("Path"), tr("You selected ") + fileName);
    QFileInfo fileInfo(fileName);
     QString file_suffix=fileName.right(fileName.size()-fileName.lastIndexOf(".")-1);
    QString no_suffix_fileName=fileInfo.completeBaseName();
    QString file_path=fileInfo.absolutePath();

    if(file_suffix=="mp4"||file_suffix=="avi"||file_suffix=="mkv"||file_suffix=="mp3"||file_suffix=="FLV"||file_suffix=="flv"||file_suffix=="WMV"||
            file_suffix=="MKV"||file_suffix=="MP4"||file_suffix=="AVI")
    {
        fileInfo_to_play=fileInfo;
        qDebug()<<"the path is"<<fileInfo.absolutePath();
        player=new Video_Player();
        on_hide_moviedesktop();
        emit main_desktop_disvisible();
//        this->setHidden(true);
        player->show();
    }else if(file_suffix=="jpg"||file_suffix=="jpeg"||file_suffix=="png"||file_suffix=="bmp"||file_suffix=="BMP")
    {
        fileInfo_to_show=fileInfo;
        pic_view=new Picture_view();
        on_hide_moviedesktop();
        emit main_desktop_disvisible();
        pic_view->show();

    }else{
        QMessageBox::information(NULL,tr("Information"), tr("Haven't support this file type ,thanks!"));
    }
}

void movieDesk::FormInCenter()
{
    int frmX = this->width();
    int frmY = this->height();
    QDesktopWidget w;
    int deskWidth = w.width();
    int deskHeight = w.height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    this->move(movePoint);
}
void movieDesk::on_returnButton_clicked()
{
    this->hide();
}
void movieDesk::on_mainCameraButton_clicked()
{
    qDebug()<<"main camera data";
    cur_index=ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(2);
    ui->tabWidget->clear();
    qDebug()<<"remove tab OK";
    ui->tabWidget->insertTab(0,videowidget,tr("video"));
    ui->tabWidget->insertTab(1,picturewidget,tr("picture"));
    ui->tabWidget->insertTab(2,fileDialog,tr("find..."));
 qDebug()<<"add tab ok";
    ui->tabWidget->setCurrentIndex(cur_index);
}
void movieDesk::on_viceButton_clicked()
{
    qDebug()<<"vice camera data";
    cur_index=ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(2);
    ui->tabWidget->clear();
    qDebug()<<"remove tab OK";
    ui->tabWidget->insertTab(0,slave_videowidget,tr("video"));
    ui->tabWidget->insertTab(1,slave_picturewidget,tr("picture"));
    ui->tabWidget->insertTab(2,fileDialog,tr("find..."));
    qDebug()<<"add tab ok";
    ui->tabWidget->setCurrentIndex(cur_index);

}
void movieDesk::on_hide_moviedesktop()
{
    this->setHidden(true);
}
void movieDesk::on_unhide_moviedesktop()
{
    this->setHidden(false);
}
