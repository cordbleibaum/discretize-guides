const { ApolloServer, gql } = require('apollo-server');
const { ObjectId, MongoClient } = require('mongodb');
const amqp = require('amqplib');
const assert = require('assert');

const typeDefs = gql`
    type CPU {
        number: Int
        manufacturer: String
        name: String
        numberOfCores: Int
        numberOfLogicalProcessors: Int
        l2CacheSize: Int
        l3CacheSize: Int
        maxClockSpeed: Int
    }

    input CPUInput {
        number: Int
        manufacturer: String
        name: String
        numberOfCores: Int
        numberOfLogicalProcessors: Int
        l2CacheSize: Int
        l3CacheSize: Int
        maxClockSpeed: Int
    }

    type Mainboard {
        manufacturer: String
        product: String
    }

    input MainboardInput {
        manufacturer: String
        product: String
    }

    type Drive {
        number: Int
        model: String
        firmwareRevision: String
        size: Int
    }

    input DriveInput {
        number: Int
        model: String
        firmwareRevision: String
        size: Int
    }

    type RAM {
        manufacturer: String
        partNumber: String
        bankLabel: String
        deviceLocator: String
        capacity: Int
        speed: Int
        configuredVoltage: Int
    }

    input RAMInput {
        manufacturer: String
        partNumber: String
        bankLabel: String
        deviceLocator: String
        capacity: Int
        speed: Int
        configuredVoltage: Int
    }   

    type GPU {
        number: Int
        videoProcessor: String
        adapterCompatibility: String
        name: String
        driverVersion: String
        adapterRAM: Int
        currentVerticalResolution: Int
        currentHorizontalResolution: Int
        currentBitsPerPixel: Int
        currentRefreshRate: Int
    }

    type GPUInput {
        number: Int
        videoProcessor: String
        adapterCompatibility: String
        name: String
        driverVersion: String
        adapterRAM: Int
        currentVerticalResolution: Int
        currentHorizontalResolution: Int
        currentBitsPerPixel: Int
        currentRefreshRate: Int
    }

    type System {
        id: ID
        mainboard: Mainboard
        cpus: [CPU]
        drives: [Drive]
        ram: [RAM],
        gpus: [GPU]
    }

    type Query {
        system(id: ID): System
    }

    type Mutation {
        createSystemID: System
        setMainboard(system: ID, mainboard: MainboardInput): Mainboard
        setCPUs(system: ID, cpus: [CPUInput]): [CPU]
        setDrives(system: ID, drives: [DriveInput]): [Drive]
        setRAMs(system: ID, rams: [RAMInput]): [RAM]
        setGPUs(system: ID, gpus: [GPUInput]): [GPU]
    }
`;

(async () => {
    const mongo_client = await MongoClient.connect('mongodb://database:27017/');
    const db = mongo_client.db('benchfactor');
    console.log('Connected to MongoDB');

    const ampq_connection = await amqp.connect('amqp://messagequeue/');
    const channel = await ampq_connection.createChannel();
    console.log('Connected to AMPQ');

    const resolvers = {
        Query: {
            system: async (_, { id }, context) => {
                const system = await db.collection('systems').findOne({
                    _id: ObjectId(id)
                })
                return {
                    id: system._id,
                    cpus: system.cpus,
                    mainboard: system.mainboard,
                    drives: system.drives
                }
            }
        },
        Mutation: {
            // TODO maintain change log
            createSystemID: async () => {
                let emptySystem = {}
                const result = await db.collection('systems').insertOne(emptySystem);
                return { "id": result.insertedId};
            },
            setCPUs: async  (_, { system, cpus }, context) => {
                await db.collection('systems').updateOne({
                    _id: ObjectId(system)
                }, {
                    $set: {
                        cpus: cpu
                    }
                }, {
                    upsert: true
                })
                return cpus;
            },
            setDrives: async  (_, { system, drives }, context) => {
                await db.collection('systems').updateOne({
                    _id: ObjectId(system)
                }, {
                    $addToSet: {
                        drives: drive
                    }
                }, {
                    upsert: true
                })
                return drives;
            },
            setRAMs: async  (_, { system, rams }, context) => {
                await db.collection('systems').updateOne({
                    _id: ObjectId(system)
                }, {
                    $set: {
                        rams: rams
                    }
                }, {
                    upsert: true
                })
                return rams;
            },
            setGPUs: async  (_, { system, gpus }, context) => {
                await db.collection('systems').updateOne({
                    _id: ObjectId(system)
                }, {
                    $set: {
                        gpus: gpus
                    }
                }, {
                    upsert: true
                })
                return gpus;
            },
            setMainboard: async  (_, { system, mainboard }, context) => {
                await db.collection('systems').updateOne({
                    _id: ObjectId(system)
                }, {
                    $set: {
                        mainboard: mainboard
                    }
                }, {
                    upsert: true
                })
                return mainboard;
            }
        }
    };

    const server = new ApolloServer({ typeDefs, resolvers });
    server.listen({
        port: 4000,
    }).then(({ url }) => {
        console.log(`🚀  Server ready at ${url}`);
    });
})();
