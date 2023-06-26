#ifndef RECVTHREADECAL_H
#define RECVTHREADECAL_H

#include "recvthread.h"
#include <QTimer>

/*!
 * \brief The RecvThreadECal class
 * 그래프 요구사항
 * - 여러개의 창(탭)에 그래프를 표시
 * - 창은 탭이나 독립 창으로 도킹 및 리사이징 가능
 * - 하나의 그래프에 복수 데이터 시리즈 표시 가능
 * - 하나의 그래프에 복수 토픽 데이터 표시 가능
 * - 데이터 시리즈는 실수형(float/double) scalar 만 지원
 * - 정수는 실수로 타입변환
 * - Bool형은 실수(0, 1)로 타입변환
 * - 데이터 추가는 그래프 생성시만 가능하며, 동적(그래프 표시되는 도중) 추가 안됨
 * - 하나의 데이터 시리즈는 다음 정보와 연결
 * -- 토픽 이름
 * -- 데이터 타입
 * -- 데이터 getter (어떻게 ????)
 * -
 *
 * ------- 수신하는 데이터(메타 데이터와 함께 데이터 파일로 저장)와 표시되는 데이터(표시용 메타 데이터만 분리하여 저장)는 분리할 필요 있음
 * ------- 플롯 프로젝트 파일이란? 표시용 메타 데이터만을 저장. 데이터는 온라인(실시간 토픽 서브스크라이브) 또는 오프라인(데이터 파일 로드)으로 받을 수 있음
 * ------- 데이터 파일이란? 독립적으로 로드할 수 있음, 데이터 파일 로드시 이전 데이터를 지우고 로드(데이터 append 기능 지원 안함)
 * ------- 실시간 플롯 기능과 데이터 뷰어(스태틱 데이터 로딩/표시) 기능을 섞으면 구현이 복잡해짐
 *
 * ------- 몇개의 그래프
 * ------- 하나의 그래프에, 어떤 토픽, 어떤 데이터
 * ------- DataSet has ... list<Graph>
 * ------- Graph has ... list<Topic>
 * ------- Topic has ... list<Data>
 *
 * 인터페이스 요구사항
 * - x축(시간축) 표시 범위 설정: 시작 시간(start time), 표시 시간(duration)
 * - y축 표시 범위 설정: default(-1.0, 1.0)
 * - x축 자동 스크롤 on/off: 기능 off 시 x축 표시 범위 설정
 * - y축 자동 스케일 on/off: 기능 off 시 y축 표시 범위 설정
 * - 마우스 모드: 이동, 확대/축소
 * -- (1) 선택
 * --- 데이터 시리즈 선택 (선택시 라인, 레전드 highlight)
 * -- (2) 이동
 * --- 그래프 영역 더블클릭시 현재 x/y 스케일 유지하면서 클릭 위치를 그래프 중앙에 표시
 * --- 그래프 영역 클릭-드래그-드랍시 x/y 축 드래그 영역 만큼 그래프 스크롤
 * -- (3) 확대/축소 모드:
 * --- 그래프 영역 더블클릭시 화면 2배 확대
 * --- 그래프 영역 클릭-드래그-드랍시 해당 영역 확대
 * --- 마우스 우클릭시 화면 2배 축소
 * - legend
 * -- 표시 on/off
 * - 현재 마우스 커서가 그래프 영역에 있으면 해당 그래프에서의 좌표를 상태바에 표시
 *
 * Plot interface 참조
 * 1. matlibplot
 * 2. matlab
 * 3. rplot
 * 그래프 및 각 그래프가 표시하는 데이터의 이름을 서버(어플리케이션)에 미리 등록하여 빌드하는 방식으로,
 * 개발 편의성과 기능 안정성 측면에서 장점은 있으나, 데이터 동적 추가 등 편의성 부족
 *
 */
class RecvThreadECal : public RecvThread
{
    Q_OBJECT
public:
    explicit RecvThreadECal(QObject *parent = nullptr);

    void run();

public:
    void Initialize();

public:
//    void OnRecv();


    QTimer* threadTimer;

signals:
    void new_data_arrived(double* data, uint32_t len);

public slots:
    void OnRecv();
};

#endif // RECVTHREADECAL_H
