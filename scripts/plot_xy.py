import pandas as pd
import matplotlib.pyplot as plt
import plotly.express as px
import plotly.graph_objects as go
import os
import tkinter as tk
from tkinter import filedialog

def select_file():
    """
    파일 선택창을 띄워 사용자가 파일을 선택하도록 하고,
    선택된 파일의 경로를 반환합니다.
    """
    root = tk.Tk()
    root.withdraw()  # Tkinter 기본 창을 숨깁니다.

    current_working_directory = os.getcwd()
    file_path = filedialog.askopenfilename(
        title="파일을 선택하세요",  # 창 제목
        initialdir="~/",           # 초기 디렉토리 (여기서는 루트 디렉토리)
                                  # 'C:/' 또는 '~/Documents' 등으로 설정 가능
        filetypes=(
            ("CSV 파일", "*.csv"),     # CSV 파일만 보이도록 필터링
            ("텍스트 파일", "*.txt"),    # 텍스트 파일도 보이도록 필터링
            ("모든 파일", "*.*")        # 모든 파일 보이도록 허용
        )
    )

    if file_path:
        print(f"선택된 파일 경로: {file_path}")
        return file_path
    else:
        print("파일 선택이 취소되었습니다.")
        return None
    
def plot_csv_data(file_path, x_column, y_column, title="CSV Data Plot", x_label="", y_label=""):
    """
    CSV 파일을 읽어 (x, y) 차트를 그립니다.

    Args:
        file_path (str): CSV 파일의 경로.
        x_column (str): x축으로 사용할 CSV 컬럼의 이름.
        y_column (str): y축으로 사용할 CSV 컬럼의 이름.
        title (str, optional): 차트의 제목. 기본값은 "CSV Data Plot".
        x_label (str, optional): x축 라벨. 기본값은 "".
        y_label (str, optional): y축 라벨. 기본값은 "".
    """
    
    if not os.path.exists(file_path):
        print(f"오류: '{file_path}' 파일을 찾을 수 없습니다.")
        return
    
    try:
        df = pd.read_csv(file_path)
    except Exception as e:
        print(f"오류: CSV 파일을 읽는 중 문제가 발생했습니다: {e}")
        return
    
    if x_column not in df.columns:
        print(f"오류: '{x_column}' 컬럼이 CSV 파일에 존재하지 않습니다. 사용 가능한 컬럼: {df.columns.tolist()}")
        return
    
    if y_column not in df.columns:
        print(f"오류: '{y_column}' 컬럼이 CSV 파일에 존재하지 않습니다. 사용 가능한 컬럼: {df.columns.tolist()}")
        return
    
    # 데이터 타입이 숫자인지 확인 (그래프를 그릴 수 있도록)
    try:
        df[x_column] = pd.to_numeric(df[x_column])
        df[y_column] = pd.to_numeric(df[y_column])
    except ValueError:
        print("오류: x 또는 y 컬럼의 데이터를 숫자로 변환할 수 없습니다. 숫자로 구성된 컬럼을 선택해주세요.")
        return
    
    #
    # matplotlib을 이용한 xy scatter plot
    #
    # plt.figure(figsize=(10, 6)) # 그래프 크기 설정
    # plt.plot(df[x_column], df[y_column], marker='o', linestyle='') # 차트 그리기
    # plt.title(title) # 제목 설정
    # plt.xlabel(x_label if x_label else x_column) # x축 라벨 설정
    # plt.ylabel(y_label if y_label else y_column) # y축 라벨 설정
    # plt.grid(True) # 그리드 표시
    # plt.show() # 차트 보여주기
    
    #
    # plotly express를 사용한 xy scatter plot
    #
    fig = px.scatter(df, x=x_column, y=y_column,
                    #  color="category",           # 'category' 컬럼에 따라 색상 구분
                    #  size="y_values",            # 'y_values' 크기에 따라 마커 크기 조절
                    #  symbol="category",          # 'category' 컬럼에 따라 마커 심볼 변경
                    #  hover_name="category",      # 마우스를 올렸을 때 표시될 이름
                     title=title,
                     labels={x_column: x_label, y_column: y_label},
                     log_x=False,                # x축 로그 스케일 사용 여부
                     log_y=False,                # y축 로그 스케일 사용 여부
                     size_max=20)                # 마커 최대 크기
    fig.show()
    
    #
    # plotly graph object를 사용한 xy scatter plot
    #
    # fig = go.Figure(
    #     data=[
    #         go.Scatter(
    #             x=df[x_column],
    #             y=df[y_column],
    #             mode='markers', # 'markers', 'lines', 'lines+markers'
    #             name='', # 범례에 표시될 이름
    #             marker=dict(
    #                 size=1,
    #                 color='red',
    #                 symbol='circle'
    #             )
    #         )
    #     ]
    # )
    # fig.update_layout(
    #     title=title,
    #     xaxis_title=x_label,
    #     yaxis_title=y_label
    # )
    # fig.show()

if __name__ == "__main__":
    
    csv_file = select_file()
    if csv_file:        
        # 차트 그리기 함수 호출
        plot_csv_data(csv_file, 'x', 'y',
                        title="X vs Y Plot from CSV",
                        x_label="X-Axis Values",
                        y_label="Y-Axis Values")