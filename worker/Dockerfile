FROM tensorflow/tensorflow:latest

COPY requirements.txt /tmp/
RUN pip install -r /tmp/requirements.txt

RUN useradd --create-home appuser
WORKDIR /home/appuser
USER appuser

COPY *.py ./

CMD [ "python", "-u", "./main.py" ]
