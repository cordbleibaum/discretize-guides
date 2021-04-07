import ptvsd
import logging

from db import connect_db

logging.basicConfig(level=logging.INFO)
ptvsd.enable_attach(address=('0.0.0.0', 5678))

if __name__ == "__main__":
    pass
