const { ApolloServer, gql } = require('apollo-server');
const MongoClient = require('mongodb').MongoClient;
const amqp = require('amqplib');
const assert = require('assert');

const typeDefs = gql`
    type System {
        id: ID!
        parts: [Part]!
    }

    type Part {
        id: ID!
        name: String
        number: String
        image: String
    }

    type Benchmark {
        id: ID!
        system: System!
    }

    type Query {
        benchmark(id: ID!): [Benchmark]
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
            benchmark: (id) => { },
        },
    };

    const server = new ApolloServer({ typeDefs, resolvers });
    server.listen({
        port: 4000,
    }).then(({ url }) => {
        console.log(`🚀  Server ready at ${url}`);
    });
})();
