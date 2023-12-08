// Included in Path.hpp

   struct StackValue {
      Path* _path = nullptr;
      bool _bit;
   };

   class Stack :
      public PowerEncoding,
      public vector<StackValue>
   {
   protected:
      Size _index {0};
      long int _count {0};

   public:
      
      Stack()
      {
      }
     
      virtual ~Stack() {
         for (auto value : *this) {
            if (value._path)
               delete value._path;
         }
      }

      virtual bool peekBit() const
      override
      {
         return (*this)[_index]._bit;
      }

      virtual bool readBit()
      override
      {

         bool value = peekBit();

         PowerEncoding::readBit();

         if (value)
            ++_count;
         else
            --_count;

         ++_index;

         return value;
      }

      virtual void writeBit(bool bit)
      override
      {
         PowerEncoding::writeBit(bit);
         if (bit)
            ++_count;
         else
            --_count;
      }

      void reset()
      {
         _index = 0;
         _count = 0;
      }

      virtual long int count() const
      {
         return _count;
      }
      

      friend ostream& operator << (ostream& out, const Stack& stack)
      {
         int index = 0;
        
         for (auto value : stack)
         {
            out << *(value._path) << ":" 
                << value._bit
                << endl;
         }

         return out;

      }
 
      StackValue last() {
         size_t size = vector<StackValue>::size();
         assert(size);
         return (*this)[size - 1];
      }

   };

