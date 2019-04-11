FROM httpd:2.4
LABEL maintainer Tatsunori Uchino <tats.u@live.jp>

# CGI対応
RUN sed -i '/LoadModule .*mod_cgid\.so$/s/#//;s/Options None$/Options ExecCGI/;/AddHandler cgi-script \.cgi$/s/#//' conf/httpd.conf

# C++ビルドに必要なものインストール
RUN apt-get update && apt-get install -y g++ make \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

COPY  ./blockranking.cpp ./httplib.hpp ./scoredata.hpp ./Makefile /root/
RUN cd /root \
  && make cgi \
  && cp ./blockranking.cgi /usr/local/apache2/cgi-bin/blockranking.cgi \
  && rm -f ./*.cpp ./*.hpp ./blockranking.cgi ./Makefile \
  && cd /usr/local/apache2 \
  && chmod 755 cgi-bin/blockranking.cgi \
  && chown root:src cgi-bin/blockranking.cgi
  