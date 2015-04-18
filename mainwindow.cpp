#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gklibs.h"
#include <QWidget>
#include <QSpinBox>
#include <QSlider>
#include "handprocess.h"
#include "objectcontrol.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
void encode(unsigned char *str, int x){
    str[0] = x >> 8;
    str[1] = x;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CvCapture *capture = cvCreateCameraCapture(CV_CAP_V4L2);
    IplImage *img = 0;
    while(!img)img = cvQueryFrame(capture);
    if(!capture) exit(0);
    HandProcess hp(img->width, img->height);
    objectcontrol obj;
    int sock_fd;
    unsigned char senddata[1024];
    struct sockaddr_in servaddr;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("create socket error: %s", strerror(errno));
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8885);
    servaddr.sin_addr.s_addr = inet_addr("192.168.191.1");
    if(::connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s", strerror(errno));
        exit(0);
    }
    while(true){
        IplImage *img = cvQueryFrame(capture);
        if(!img) continue;
        hp.otsuThreshold(img);
        cvShowImage("dst", hp.img);
        hp.findMaxContour();
        if(hp.isHand){
            hp.getHandCenter();
            hp.detectFingerNumber();
            obj.setPos(hp.handCenter, hp.dist);
            int len = 0;
            encode(senddata + len, hp.handCenter.x);
            len += 2;
            encode(senddata + len, hp.handCenter.y);
            len += 2;
            encode(senddata + len, hp.finger);
            len += 2;
            for(int i = 0; i < hp.finger; i++){
                encode(senddata+ len, hp.fingerTips[i].x);
                len += 2;
                encode(senddata+ len, hp.fingerTips[i].y);
                len += 2;
            }
            encode(senddata+len, 123);
            len += 2;
            printf("len %d\n", len);
            for(int i = 0; i < len; i+=2){
                printf("%d\n", (((int)senddata[i]<<8)+senddata[i+1]));
            }
            if(send(sock_fd, senddata, len, 0) != len){
                printf("send error: %s", strerror(errno));
                exit(0);
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
