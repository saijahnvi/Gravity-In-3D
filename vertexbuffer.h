class vertexbuffer{
    private:
          unsigned int m_RendererID;

    public:
        vertexbuffer(const void* data,unsigned int size);//public constructor
        ~vertexbuffer();//public destructor

        void Bind() const;
        void UnBind() const;



};