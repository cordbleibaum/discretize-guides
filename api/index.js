const { ApolloServer, gql } = require('apollo-server');
const MongoClient = require('mongodb').MongoClient;
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

const db_client = new MongoClient('mongodb://database:27017/');
db_client.connect(function (err) {
    assert.strictEqual(null, err);
    console.log('Connected successfully to database server');
    const db = db_client.db('benchfactor');

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
});
