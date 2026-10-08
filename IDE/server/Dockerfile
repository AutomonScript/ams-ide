FROM node:20-slim

RUN apt update && apt install -y openssl libssl-dev

WORKDIR /app

COPY package*.json ./
RUN npm install

COPY . .

ENV PRISMA_CLIENT_ENGINE_TYPE=binary
ENV PRISMA_CLI_QUERY_ENGINE_TYPE=binary

RUN npx prisma generate

EXPOSE 3001

CMD ["npx", "tsx", "watch", "src/index.ts"]
