
   struct StackValue {
      Path<Encoding> _path;
      bool _bit;
   };

   class Stack :
      public Encoding,
      public vector<StackValue>
   {
   protected:
      Size _index {0};
      long int _count {0};

   public:
      enum class Aggregate {
         MIN,
         MAX
      } _aggregate = Aggregate::MIN;

      Stack()
      {
      }
     
      virtual bool peekBit() const
      override
      {
         return (*this)[_index]._bit;
/*
         const Branch& branch =
            (*this)[_index].getBranch();
      
         if (_aggregate ==
            Aggregate::MIN)
         {
            if (branch._left)
               return 0;

            if (branch._right)
               return 1;
         }
         else if (_aggregate ==
                  Aggregate::MAX)
         {
            if (branch._right)
               return 1;

            if (branch._left)
               return 0;
         }
      
         assert(false);
*/
      }

      virtual bool readBit()
      override
      {

         bool value = peekBit();

         Encoding::readBit();

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
         Encoding::writeBit(bit);
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
      override
      {
         return _count;
      }
      

      friend ostream& operator << (ostream& out, const Stack& stack)
      {
         int index = 0;
        
         for (auto value : stack)
         {
            out << value._path << ":" 
                << value._bit
                << endl;
         }

         return out;

      }
 
      Path<Encoding> last() {
         size_t size = vector<StackValue>::size();
         assert(size);
         return (*this)[size - 1]._path;
      }

   };

