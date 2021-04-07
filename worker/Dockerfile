FROM tensorflow/tensorflow:latest

COPY requirements.txt /tmp/

RUN useradd --create-home appuser
WORKDIR /home/appuser
USER appuser

COPY *.py ./

CMD [ "python", "-u", "./main.py" ]
