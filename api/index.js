const { ApolloServer, gql } = require('apollo-server');

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

const resolvers = {
    Query: {
        benchmark: (id) => {},
    },
};

const server = new ApolloServer({ typeDefs, resolvers });

server.listen({
    port: 4000,
}).then(({ url }) => {
    console.log(`🚀  Server ready at ${url}`);
});
