const { ApolloServer, gql } = require('apollo-server');

const typeDefs = gql`
    type System {
        id: String
    }

    type Benchmark {
        id: String
        system: System
    }

    type Query {
        benchmarks: [Benchmark]
    }
`;

const resolvers = {
    Query: {
        benchmarks: () => [],
    },
};

const server = new ApolloServer({ typeDefs, resolvers });

server.listen({
    port: 4000,
}).then(({ url }) => {
    console.log(`🚀  Server ready at ${url}`);
});
