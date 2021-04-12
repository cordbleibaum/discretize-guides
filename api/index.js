const { ApolloServer, gql } = require('apollo-server');
const { ObjectId, MongoClient } = require('mongodb');
const amqp = require('amqplib');
const assert = require('assert');

const typeDefs = gql`
    type System {
        id: ID
    }

    type Query {
        system(id: ID): System
    }

    type Mutation {
        createSystemID: System
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
                    id: system._id
                }
            }
        },
        Mutation: {
            createSystemID: async () => {
                let emptySystem = {}
                const result = await db.collection('systems').insertOne(emptySystem);
                return { "id": result.insertedId};
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
