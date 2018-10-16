#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>  //OpenGL/GLUT
#include  <opencv2/opencv.hpp> //OpenCV
#define TILE 50  //床頂点数

//三次元ベクトル構造体: Vec_3D
typedef struct _Vec_3D
{
    double x, y, z;
} Vec_3D;

//関数名の宣言
void initGL();
void display();
void resize(int w, int h);
void timer(int value);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glDepthMask(GLboolean flag);
Vec_3D normcrossprod(Vec_3D v1, Vec_3D v2);
double vectorNormalize(Vec_3D* vec);
Vec_3D diffVec(Vec_3D v1, Vec_3D v2);

//グローバル変数
Vec_3D fPoint[TILE][TILE];  //床頂点
double fWidth;  //床長
double eDist, eDegX, eDegY;  //視点極座標
int winW, winH;  //ウィンドウサイズ
int mButton, mState, mX, mY;  //マウス情報

//メイン関数
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);  //OpenGL,GLUTの初期化

    initGL();  //初期設定

    glutMainLoop();  //イベント待ち無限ループ突入

    return 0;
}

//初期設定関数
void initGL()
{
    //ウィンドウ生成
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);  //ディスプレイ表示モード指定
    glutInitWindowSize(800, 600);  //ウィンドウサイズの指定（800×600）
    glutCreateWindow("CG09");  //ウィンドウ生成

    //コールバック関数指定
    glutDisplayFunc(display);  //ディスプレイコールバック関数（"display"）
    glutReshapeFunc(resize);  //リサイズコールバック関数（"resize"）
    glutTimerFunc(33, timer, 0);  //タイマーコールバック関数（"timer", 33ミリ秒）
    glutMouseFunc(mouse);  //マウスクリックコールバック関数
    glutMotionFunc(motion);  //マウスドラッグコールバック関数
    glutKeyboardFunc(keyboard);  //キーボードコールバック関数

    //各種設定
    glClearColor(0.0, 0.0, 0.2, 1.0);  //ウィンドウクリア色の指定（RGBA値）
    glEnable(GL_DEPTH_TEST);  //デプスバッファ有効化
    glEnable(GL_NORMALIZE);  //ベクトル正規化有効化
    glEnable(GL_BLEND); //アルファブレンディング有効化
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
    glAlphaFunc(GL_GREATER, 0.6); //アルファ値比較関数の指定

    //光源
    GLfloat col[4];
    col[0] = 0.9; col[1] = 0.9; col[2] = 0.9; col[3] = 1.0;
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
    glLightfv(GL_LIGHT0, GL_SPECULAR, col);
    col[0] = 0.1; col[1] = 0.1; col[2] = 0.1; col[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_AMBIENT, col);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001);

    glEnable(GL_LIGHT1);
    col[0] = 0.9; col[1] = 0.9; col[2] = 0.9; col[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_DIFFUSE, col);
    glLightfv(GL_LIGHT1, GL_SPECULAR, col);
    col[0] = 0.1; col[1] = 0.1; col[2] = 0.1; col[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_AMBIENT, col);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001);

    //床頂点座標
    fWidth = 100.0;
    for (int j=0; j<TILE; j++) {
        for (int i=0; i<TILE; i++) {
            fPoint[i][j].x = -fWidth/2.0+i*fWidth/(TILE-1);
            fPoint[i][j].y = 0.0;
            fPoint[i][j].z = -fWidth/2.0+j*fWidth/(TILE-1);
        }
    }

    //視点極座標
    eDist = 120.0;  //距離
    eDegX = 20.0; eDegY = 0.0;  //x軸周り角度，y軸周り角度

    //テクスチャ準備
    cv::Mat textureImage; //テクスチャ画像用配列

    //テクスチャ0
    textureImage = imread("DPrZhOJVoAArkmU.jpg", cv::IMREAD_UNCHANGED); //画像読み込み
    if(textureImage.data == 0){
        printf("Texture file not found\n");
    }
    glBindTexture(GL_TEXTURE_2D, 1); //テクスチャオブジェクトの生成(#0)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //ピクセル格納モード
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //テクスチャ拡大時の補間方法
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //テクスチャ縮小時の補間方法
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.cols, textureImage.rows, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, textureImage.data); //画像をテクスチャに割り当て





    //テクスチャ1
    textureImage = imread("/Users/x16057xx/CGプログラミング/9回/01.jpg", cv::IMREAD_UNCHANGED); //画像読み込み
    if(textureImage.data == 0){
    printf("Texture file not found\n");
    }
    glBindTexture(GL_TEXTURE_2D, 1); //テクスチャオブジェクトの生成(#1)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //ピクセル格納モード
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //テクスチャ拡大時の補間方法
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //テクスチャ縮小時の補間方法
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.cols, textureImage.rows, 0, GL_BGR,GL_UNSIGNED_BYTE, textureImage.data); //画像をテクスチャに割り当て

    //テクスチャ2
    textureImage = imread("V4S.png", cv::IMREAD_COLOR); //画像読み込み
    if(textureImage.data == 0){
        printf("Texture file not found\n");
    }
    glBindTexture(GL_TEXTURE_2D, 2); //テクスチャオブジェクトの生成(#2)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //ピクセル格納モード
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //テクスチャ拡大時の補間方法
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //テクスチャ縮小時の補間方法
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.cols, textureImage.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, textureImage.data); //画像をテクスチャに割り当て

}

//リサイズコールバック関数(w：ウィンドウ幅，h：ウィンドウ高さ)
void resize(int w, int h)
{
    glViewport(0, 0, w, h);  //ビューポート設定（ウィンドウ全体を表示領域に設定）

    //投影変換
    glMatrixMode(GL_PROJECTION);  //カレント行列の設定（投影変換行列）
    glLoadIdentity();  //カレント行列初期化
    gluPerspective(30.0, (double)w/(double)h, 1.0, 1000.0);  //透視投影のための投影変換行列の生成

    winW = w; winH = h;  //ウィンドウサイズをグローバル変数に格納
}

//ディスプレイコールバック関数
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //ウィンドウクリア

    //ビューイング変換準備
    glMatrixMode(GL_MODELVIEW);  //カレント行列の設定（モデルビュー変換行列）
    glLoadIdentity();  //行列初期化
    //視点極座標から直交座標へ変換
    Vec_3D e;
    e.x = eDist*cos(eDegX*M_PI/180.0)*sin(eDegY*M_PI/180.0);
    e.y = eDist*sin(eDegX*M_PI/180.0);
    e.z = eDist*cos(eDegX*M_PI/180.0)*cos(eDegY*M_PI/180.0);
    //視点設定・ビューイング変換
    if(cos(eDegX*M_PI/180.0) < 0.0){  //ジンバルロック回避
        gluLookAt(e.x, e.y, e.z, 0.0, 0.0, 0.0, 0.0,-1.0, 0.0);  //視点視線設定（視野変換行列を乗算）
    }else{
        gluLookAt(e.x, e.y, e.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  //視点視線設定（視野変換行列を乗算）
    }

    GLfloat lightPos0[] = {-10.0, 15.0, 15.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    GLfloat lightPos1[] = {15.0, 10.0, 15.0, 1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    GLfloat col[4], spe[4], shi[1];

    //----------床パネル----------
    col[0] = 0.0; col[1] = 0.5; col[2] = 0.0; col[3] = 1.0;
    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
    shi[0] = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
    glBegin(GL_QUADS);  //図形開始
    for (int j=0; j<TILE-1; j++) {
        for (int i=0; i<TILE-1; i++) {
            Vec_3D v1, v2, n;
            v1 = diffVec(fPoint[i][j+1], fPoint[i][j]);
            v2 = diffVec(fPoint[i+1][j], fPoint[i][j]);
            n = normcrossprod(v1, v2);
            glNormal3d(n.x, n.y, n.z);
            glVertex3d(fPoint[i][j].x, fPoint[i][j].y, fPoint[i][j].z);  //頂点座標
            glVertex3d(fPoint[i][j+1].x, fPoint[i][j+1].y, fPoint[i][j+1].z);  //頂点座標
            glVertex3d(fPoint[i+1][j+1].x, fPoint[i+1][j+1].y, fPoint[i+1][j+1].z);  //頂点座標
            glVertex3d(fPoint[i+1][j].x, fPoint[i+1][j].y, fPoint[i+1][j].z);  //頂点座標
        }
    }
    glEnd();  //図形終了

//    //物体1（立方体）
//    glPushMatrix();  //行列一時保存
//    col[0] = 0.0; col[1] = 0.0; col[2] = 1.0; col[3] = 1.0;
//    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
//    shi[0] = 50;
//    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
//    glTranslated(-15.0, 5.0, -10.0);  //平行移動
//    glScaled(5.0, 5.0, 5.0);  //拡大
//    glutSolidCube(2.0);  //立方体
//    glPopMatrix();  //行列復帰

//    //物体2（ティーポット）
//    glPushMatrix();  //行列一時保存
//    col[0] = 1.0; col[1] = 0.5; col[2] = 0.0; col[3] = 1.0;
//    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
//    shi[0] = 50;
//    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
//    glTranslated(15.0, 5.0, -10.0);  //平行移動
//    glScaled(5.0, 5.0, 5.0);  //拡大
//    glutSolidTeapot(1.0);  //ティーポット
//    glPopMatrix();  //行列復帰



    //物体3（長方形パネル1）
    glEnable(GL_ALPHA_TEST); //アルファテスト有効化
    glEnable(GL_TEXTURE_2D); //テクスチャ有効化
    glBindTexture(GL_TEXTURE_2D, 2); //テクスチャオブジェクト指定(#2)
    glPushMatrix();  //行列一時保存
    col[0] = 1.0; col[1] = 1.0; col[2] = 1.0; col[3] = 1.0;
    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
    shi[0] = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
    glTranslated(-25.0, 15.0, 10.0);  //平行移動
    glRotated(40.0, 0.0, 1.0, 0.0); //回転移動
    glScaled(20.0, 15.0, 1.0);  //拡大
    glNormal3d(0.0, 0.0, 1.0);  //法線
    glBegin(GL_QUADS);  //四角形
    glTexCoord2d(0.0, 0.0); //テクスチャ座標0
    glVertex3d(-0.5, 0.5, 0.0);  //頂点0
    glTexCoord2d(0.0, 1.0); //テクスチャ座標1
    glVertex3d(-0.5, -0.5, 0.0);  //頂点1
    glTexCoord2d(1.0, 1.0); //テクスチャ座標2
    glVertex3d(0.5, -0.5, 0.0);  //頂点2
    glTexCoord2d(1.0, 0.0); //テクスチャ座標3
    glVertex3d(0.5, 0.5, 0.0);  //頂点3
    glEnd();
    glPopMatrix();  //行列復帰
    glDisable(GL_TEXTURE_2D); //テクスチャ無効化
    glDisable(GL_ALPHA_TEST); //アルファテスト無効化

    //物体4（長方形パネル2）
    glEnable(GL_ALPHA_TEST); //アルファテスト有効化
    glEnable(GL_TEXTURE_2D); //テクスチャ有効化
    glBindTexture(GL_TEXTURE_2D, 0); //テクスチャオブジェクト指定(#0)
    glPushMatrix();  //行列一時保存
    col[0] = 1.0; col[1] = 1.0; col[2] = 1.0; col[3] = 1.0;
    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
    shi[0] = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
    glTranslated(25.0, 15.0, 10.0);  //平行移動
    glRotated(-40.0, 0.0, 1.0, 0.0); //回転移動
    glScaled(20.0, 15.0, 1.0);  //拡大
    glNormal3d(0.0, 0.0, 1.0);  //法線
    glBegin(GL_QUADS);  //四角形
    glTexCoord2d(0.0, 0.0); //テクスチャ座標0
    glVertex3d(-0.5, 0.5, 0.0);  //頂点0
    glTexCoord2d(0.0, 1.0); //テクスチャ座標1
    glVertex3d(-0.5, -0.5, 0.0);  //頂点1
    glTexCoord2d(1.0, 1.0); //テクスチャ座標2
    glVertex3d(0.5, -0.5, 0.0);  //頂点2
    glTexCoord2d(1.0, 0.0); //テクスチャ座標3
    glVertex3d(0.5, 0.5, 0.0);  //頂点3
    glEnd();
    glPopMatrix();  //行列復帰
    glDisable(GL_TEXTURE_2D); //テクスチャ無効化
    glDisable(GL_ALPHA_TEST); //アルファテスト無効化
//    //物体5（球）
//    glPushMatrix();  //行列一時保存
//    col[0] = 1.0; col[1] = 0.0; col[2] = 0.0; col[3] = 1.0;
//    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
//    shi[0] = 50;
//    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi);
//    glTranslated(0.0, 5.0, -10.0);  //平行移動
//    glScaled(5.0, 5.0, 5.0);  //拡大
//    glutSolidSphere(1.0, 36, 18);  //球
//    glPopMatrix();  //行列復帰

    //物体 6(長方形パネル 3)
    glEnable(GL_ALPHA_TEST); //アルファテスト有効化
    glEnable(GL_TEXTURE_2D); //テクスチャ有効化
    glBindTexture(GL_TEXTURE_2D, 1); //テクスチャオブジェクト指定(#1)
    glPushMatrix(); //行列一時保存
    col[0] = 1.0; col[1] = 1.0; col[2] = 1.0; col[3] = 1.0;
    spe[0] = 1.0; spe[1] = 1.0; spe[2] = 1.0; spe[3] = 1.0;
    shi[0] = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col); glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col); glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe); glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shi); glTranslated(0.0, 15.0, 0.0); //平行移動
    glScaled(20.0, 15.0, 1.0); //拡大
    glNormal3d(0.0, 0.0, 1.0); //法線
    glBegin(GL_QUADS); //四角形
    glTexCoord2d(0.0, 0.0);
    glVertex3d(-0.5,0.5,0.0); //頂点0
    glTexCoord2d(0.0, 1.0);
    glVertex3d(-0.5,-0.5,0.0); //頂点1
    glTexCoord2d(1.0, 1.0);
    glVertex3d(0.5,-0.5,0.0); //頂点2
    glTexCoord2d(1.0, 0.0);
    glVertex3d(0.5,0.5,0.0); //頂点3
    glEnd();
    glPopMatrix(); //行列復帰
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST); //アルファテスト無効化

    glutSwapBuffers();  //OpenGLの命令実行
}

//タイマーコールバック関数
void timer(int value)
{
    glutPostRedisplay();  //ディスプレイイベント強制発生
    glutTimerFunc(33, timer, 0);  //タイマー再設定
}

//マウスクリックコールバック関数
void mouse(int button, int state, int x, int y)
{
    //マウス情報をグローバル変数に格納
    mButton = button; mState = state; mX = x; mY = y;
}

//マウスドラッグコールバック関数
void motion(int x, int y)
{
    //左ボタンドラッグでチルト・パン変更
    if(mButton==GLUT_LEFT_BUTTON){
        eDegY += (mX - x)*0.1;  //マウス横方向→水平角
        eDegX += (y - mY)*0.1;  //マウス縦方向→垂直角
    }
    //右ボタンドラッグでズーム変更
    if(mButton==GLUT_RIGHT_BUTTON){
        GLfloat winX, winY, winZ;  //ウィンドウ座標
        GLdouble objX, objY, objZ;  //ワールド座標
        GLdouble model[16], proj[16];  //モデルビュー変換行列，投影変換行列格納用
        GLint view[4];  //ビューポート設定格納用

        //マウス座標→ウィンドウ座標
        winX = x;
        winY = winH-y;
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        //変換行列取り出し
        glGetDoublev(GL_MODELVIEW_MATRIX, model);  //モデルビュー変換行列格納
        glGetDoublev(GL_PROJECTION_MATRIX, proj);  //投影変換行列格納
        glGetIntegerv(GL_VIEWPORT, view);  //ビューポート設定格納

        //ウィンドウ座標→ワールド座標（objX, objY, objZ）
        gluUnProject(winX, winY, winZ, model, proj, view, &objX, &objY, &objZ);

        //(fPoint[0][0].x,fPoint[0][0].y,fPoint[0][0].z)と(objX, objY, objZ)の距離
        for (int j=0; j<TILE; j++) {
            for (int i=0; i<TILE; i++) {
                double dist = sqrt(pow(fPoint[i][j].x-objX, 2)+pow(fPoint[i][j].y-objY, 2)+pow(fPoint[i][j].z-objZ, 2));

                if (dist<10) {
                    //床面を持ち上げる
                    fPoint[i][j].y += (10-dist)*0.1;
                }
                //printf("Dist = %f\n", dist);
            }
        }
    }

    //マウス座標をグローバル変数に保存
    mX = x; mY = y;
}

void keyboard(unsigned char key, int x, int y)
{
    printf("KEY: ASCII code=%d, x=%d, y=%d\n", key, x, y);
    switch (key) {
        case 27:  //[ESC]キー
        case 'q':  //[q]キー
        case 'Q':  //[Q]キー
            exit(0);  //プロセス終了
            break;

        default:
            break;
    }
}

//ベクトルの外積計算
Vec_3D normcrossprod(Vec_3D v1, Vec_3D v2)
{
    Vec_3D out;

    out.x = v1.y*v2.z-v1.z*v2.y;
    out.y = v1.z*v2.x-v1.x*v2.z;
    out.z = v1.x*v2.y-v1.y*v2.x;
    vectorNormalize(&out);

    return out;
}

//ベクトルの差
Vec_3D diffVec(Vec_3D v1, Vec_3D v2)
{
    Vec_3D out;

    out.x = v1.x-v2.x;
    out.y = v1.y-v2.y;
    out.z = v1.z-v2.z;

    return out;
}

//ベクトル正規化
double vectorNormalize(Vec_3D* vec)
{
    double length;

    //ベクトル長
    length = sqrt(vec->x*vec->x+vec->y*vec->y+vec->z*vec->z);

    if (length>0) {
        //正規化
        vec->x = vec->x/length;
        vec->y = vec->y/length;
        vec->z = vec->z/length;
    }
    //戻り値はベクトル長
    return length;
}
