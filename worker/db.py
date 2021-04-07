import pymongo

def connect_db():
    db_client = pymongo.MongoClient("mongodb://database:27017/")

    return db_client["benchfactor"]
