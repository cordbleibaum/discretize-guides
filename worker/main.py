import ptvsd
import logging
import pika
import pymongo
import signal
import sys

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    ptvsd.enable_attach(address=('0.0.0.0', 5678))
    db = pymongo.MongoClient("mongodb://database:27017/")

    channel = None

    def on_connected(connection):
        connection.channel(on_open_callback=on_channel_open)

    def on_channel_open(new_channel):
        global channel
        channel = new_channel
        channel.queue_declare(queue="worker", durable=True, exclusive=False, auto_delete=False, callback=on_queue_declared)

    def on_queue_declared(frame):
        channel.basic_consume('worker', handle_delivery)

    def handle_delivery(channel, method, header, body):
        print(body)

    parameters = pika.ConnectionParameters('messagequeue')
    connection = pika.SelectConnection(parameters, on_open_callback=on_connected)

    def handle_sigterm(*args):
        logging.info("Received SIGTERM, shutting down")
        connection.close()
        connection.ioloop.start()
        sys.exit(0)

    signal.signal(signal.SIGTERM, handle_sigterm)

    try:
        connection.ioloop.start()
    except KeyboardInterrupt:
        logging.info("Received KeyboardInterrrupt, shutting down")
        connection.close()
        connection.ioloop.start()
