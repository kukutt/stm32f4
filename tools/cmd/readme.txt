# 创建新环境配置 可视化需要用3.6版本
conda create -n adda python=3.6 # 创建新的虚拟环境
conda activate adda	# 激活环境

# 配置代理
conda config --set proxy_servers.http "http://127.0.0.1:10809/"
conda config --set proxy_servers.https "http://127.0.0.1:10809/"
conda config --set ssl_verify false

# 安装
pip install matplotlib
pip install scipy
pip install pyserial
