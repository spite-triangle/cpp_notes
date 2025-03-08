# ORM 框架

# 概念

**对象关系映射 `Object Relational Mapping (ORM)`** : 是一种为了解决面向对象与关系数据库存在的互不匹配的现象的技术，即通过对象来完成数据库操作。`ORM` 框架是连接数据库的桥梁，只要提供了持久化类与表的映射关系，`ORM` 框架在运行时就能参照映射文件的信息，把对象持久化到数据库中。
- `ORM` 类对应数据库中的一张表
- `ORM` 类属性对应表中的字段
- `ORM` 类实例对应表中的一条记录

通过 ORM 不仅可以简化数据库操作，还能实现程序与数据库分离解耦。在 Django 中只使用一套接口，便能实现对 `MySQL`、`SQLITE`、`PostgreSQL` 等支持。

# 配置数据库

1. 安装数据库对应驱动

```term
triangle@LEARN:~$ pip install psycopg2    // postgresql 驱动，报错了就装下面的
triangle@LEARN:~$ pip install psycopg2-binary
triangle@LEARN:~$ pip install pymysql // mysql 驱动
```

2. `settings.py` 配置数据

```python
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.postgresql',
        'NAME': 'your_database_name',
        'USER': 'your_database_user',
        'PASSWORD': 'your_database_password',
        'HOST': 'localhost',
        'PORT': '5432',
    },
    'other' :{
        '..' : '...'
    }
}
```

# 模型定义

- `models.py`

```python
from django.db import models

class BookTypeInfo(models.Model):
    # 使用 AutoField 定义自增主键
    id = models.AutoField(primary_key=True)
    bookTypeName = models.CharField(max_length=20)
    class Meta:
        # 表明
        db_table = "t_bookType"
        # 方便 admin 管理
        verbose_name = "图书类别"

class BookInfo(models.Model):
    id = models.AutoField(primary_key=True)
    bookName = models.CharField(max_length=20)
    price = models.FloatField()
    publishDate = models.DateField()

    # 定义外键，实现 BookInfo 关联 BookTypeInfo
    # NOTE - 数据库中存储的字段名为 `bookType_id`
    bookType = models.ForeignKey(to=BookTypeInfo, on_delete=models.PROTECT)

    class Meta:
        db_table = "t_book"
        verbose_name = "图书信息"
```

| 字段                       | 描述                                               |
| -------------------------- | -------------------------------------------------- |
| AutoField                  | 自增长类型，数据表的字段类型为整数，长度为11位。   |
| BigAutoField               | 自增长类型，数据表的字段类型为bigint，长度为20位。 |
| CharField                  | 字符类型。                                         |
| BooleanField               | 布尔类型。                                         |
| CommaSeparatedIntegerField | 用逗号分割的整数类型。                             |
| DateField                  | 日期（Date）类型。                                 |
| DateTimeField              | 日期时间( Datetime)类型。                          |
| Decimal                    | 十进制小数类型。                                   |
| EmailField                 | 字符类型,存储邮箱格式的字符串。                    |
| FloatField                 | 浮点数类型，数据表的字段类型变成Double类型。       |
| IntegerField               | 整数类型，数据表的字段类型为11位的整数。           |
| BigIntegerField            | 长整数类型。                                       |
| IPAddressField             | 字符类型，存储Ipv4地址的字符串。                   |
| GenericIPAddressField      | 字符类型，存储Ipv4和Ipv6地址的字符串。             |
| NullBooleanField           | 允许为空的布尔类型。                               |
| PositiveIntegerFiel        | 正整数的整数类型。                                 |
| PositiveSmallIntegerField  | 小正整数类型，取值范围为0~32767。                  |
| SlugField                  | 字符类型，包含字母、数字、下画线和连字符的字符串。 |
| SmallIntegerField          | 小整数类型，取值范围为-32,768~+32,767。            |
| TextField                  | 长文本类型。                                       |
| TimeField                  | 时间类型                                           |
| URLField                   | 字符类型，存储路由格式的字符串。                   |
| BinaryField                | 二进制数据类型。                                   |
| FileField                  | 字符类型，存储文件路径的字符串。                   |
| ImageField                 | 字符类型，存储图片路径的字符串。                   |
| FilePathField              | 字符类型，从特定的文件目录选择某个文件。           |

# 模型迁移

```term
triangle@LEARN:~$ python manage.py makemigrations // 去检查创建或修改数据库，如果不合法会给出提示，且生成迁移脚本
triangle@LEARN:~$ python manage.py migrate // 执行数据库迁移脚本
```


# 数据查询

## 基本查询

```python
from django.db.models import Q
from .models import BookInfo,BookTypeInfo

def bookList(request):
    title = '图书列表'

    # 查询所有数据
    # 返回结果为 QuerySet，是类似存储 BookInfo 实例的数组
    bookList = BookInfo.objects.all()
    bookList[0]     # 索引取值
    bookList[:2]    # 切片取值，ppython 语法

    # 只读取指定字段
    BookInfo.objects.values('bookName', 'price')        # 返回 QuerySet
    BookInfo.objects.values_list('bookName', 'price')   # 类似 字段组成的元组的数据

    # 根据主键查询单条数据
    BookInfo.objects.get(id=2)     # 返回 BookInfo

    # 'where id =2' 筛选
    BookInfo.objects.filter(id=2)   # 返回 QuerySet

    d = dict(price=100, id=1)
    BookInfo.objects.filter(**d)    # 字典作为输入

    # 'where id = 1 or price = 88'
    BookInfo.objects.filter(Q(id=1) | Q(price=88))

    #'where not (id=1)' 筛选，'~' 表示取反
    BookInfo.objects.filter(~Q(id=1))
    BookInfo.objects.exclude(Q(id=1))

    # 排序
    BookInfo.objects.order_by('id')

    # QuerySet 的常用方法
    bookList.count()        # 数量统计
    bookList.distinct()     # 去重

    # annotate 实现聚合查询，即 `group by`
    # SQL: select bookType_id，SUM(price) AS 'price_sum' from t_book GROUP BY bookType_id
    BookInfo.objects.values('bookType').annotate(Sum('price'))     # 根据 bookType_id 分组，然后将 price 进行求和 

    content_value = {'bookList': bookList}
    return render(request, 'bookList.html', context=content_value)
```

> [!tip]
> - `get()` : 只能查询主键或唯一索引字段，且字段不存在会报错
> - `filter()` : 不会报错且没有字段限制，兼容性更好，更推荐使用

- `bookList.html`

```html
{% for book in bookList %}
    <tr>
        <td>{{ book.id }}</td>
        <td>{{ book.bookName }}</td>
        <td>{{ book.price }}</td>
        <td>{{ book.publishDate | date:"Y-m-d" }}</td>
        <td>{{ book.bookType.bookTypeName }}</td>
    </tr>
{% endfor %}
```

## 分页查询

```python
from django.core.paginator import Paginator
from .models import BookInfo,BookTypeInfo

def split_query(request):
    # 生成一个 QuerySet
    bookList = BookInfo.objects.order_by('id')

    # Paginator(object_list ,per_page)
    # - object_list   结果集/列表
    # - per_page  每页多少条记录
    p = Paginator(bookList, 2)

    # 获取第几页的数据
    bookListPage = p.page(2)
    
    return HttpResponse('ok')
```

## 高级查询

```python
# 1.  等于 xxxx__exact
# 例: 查询name等于‘xiao ming’的学生
Student.objects.filter(name='xiao ming')
Student.objects.filter(name__exact='xiao ming')#此处的exact可以省略

# 2.  模糊查询 xxxx_contains
# 例：查询姓名包含'xiao'的学生。
Student.objects.filter(name__contains='xiao')

# 开头：startswith  结尾：endswith
# 例：查询姓名以'xiao'开头的学生 以'ming'结尾的学生
Student.objects.filter(name__startswith='xiao')
Student.objects.filter(name__endswith='ming')

# 3.  空查询   xxxx__isnull
# 例：查询姓名不为空的学生
Student.objects.filter(name__isnull=False)

# 4.  范围查询  xxxx__in 
# 例：查询年龄12或15或16的学生
Student.objects.filter(age__in=[12,15,16])

# 5.  比较查询 
# - gt大于 
# - lt(less than) 
# - gte(equal) 
# - lte
# 例：查询年龄大于等于12的学生
Student.objects.filter(age__gte=12)

# 6.  日期查询  date
# 例：查询1994年出生的学生。
Student.objects.filter(birthyear__date=1994)
# 例：查询1994年1月1日后出生的学生。
Student.objects.filter(birthyear__date__gt = date(1994,1,1))

# 7. 返回不满足条件的数据 exclude
# 例：查询id不为3的学生。
Student.objects.exclude(id=3)
```

## 多表查询

上文通过定义外键 `BookInfo.bookType` 将表 `BookInfo` 与 `BookTypeInfo` 关联上，可以实现多表的关联查询

```python
# 正向查询，BookInfo 实例通过 bookType 查询 BookTypeInfo
book = BookInfo.objects.filter(id=2).first()
book.bookType.bookTypeName

# 反向查询，通过 BookTypeInfo 实例查询所有关联了该实例的 BookInfo 实例
bookType = BookTypeInfo.objects.filter(id=1).first()
bookType.bookinfo_set # xxxx_set，xxxx 为定义关联外键的模型类名，全部为小写
```

# 新增/修改数据

```python
from .models import BookInfo,BookTypeInfo

def add_update(request):

    # 定义数据实例
    book = BookInfo()
    book.id = request.POST.get("id")
    book.bookName = request.POST.get("bookName")
    book.publishDate = request.POST.get("publishDate")
    book.price = request.POST.get("price")

    # NOTE - 在实际数据库中外键字段名是 `bookType_id`
    book.bookType_id = request.POST.get("bookType_id")

    # 判断 book.id 是否存在
    # - 存在，修改
    # - 不存在，新增
    book.save()

    # 数据添加后，获取新增数据的主键id
    book.id
    return HttpResponse('ok')
```

# 删除数据


```python

# 删除全部
BookInfo.objects.all().delete()

# 删除一条
BookInfo.objects.get(id=1).delete()

# 过滤删除
BookInfo.objects.filter(price__gte=90).delete()
```

# 执行 SQL

ORM 框架支持三种不同层次的 SQL 执行方式

- `extra` : 适合用于 ORM 难以实现的查询条件，将查询条件使用原生 SQL 语法实现

    ```python
    # - select: 添加新的查询字段，即新增并定义模型之外的字段。
    # - select_params: 给 select 格式化语句提供数值
    # - where: 设置查询条件。
    # - params: 给 where 格式化语句提供数值
    # - tables: 连接其他数据表，实现多表查询。
    # - order_by: 设置数据的排序方式。
    bookList = BookInfo.objects.extra(where=["price > %s"], params=[90])
    ```

- `raw` : 只能实现数据查询操作，直接拼写 SQL 查询语句

    ```python
    # raw_query : SQL语句。
    # params : 为 raw_query 格式化字符串提供参数
    # translations : 为查询的字段设置别名。
    # using : 数据库对象，即 Django 所连接的数据库。
    bookList = BookInfo.objects.raw(raw_query="select * from t_book where price > %s", params=[90])
    ```

- `execute` : 绕过 ORM 框架，直接执行原生的 SQL 语句

    ```python
    from django.db import connection
    from django.db.backends.utils import CursorDebugWrapper

    cursor : CursorDebugWrapper = connection.cursor() 
    cursor.execute("select count(*) from t_book where price > 90")
    cursor.fetchone()
    ```

# 事务

**事务** ： 保证事务中包含的所有操作都保持原子性，要么都成功，要么都失败。

```python
from django.db.models import F
from django.db import transaction

# 启用事务
@transaction.atomic
def transfer(request):
    # 开启事务
    sid = transaction.savepoint()

    try:
        a1 = AccountInfo.objects.filter(user='张三')
        # F('account') : 获取旧值
        # 该写法同 a1.account = a1.account + 100
        a1.update(account=F('account') + 100)

        a2 = AccountInfo.objects.filter(user='李四')
        a2.update(account=F('account') - 100 / 0)

        # 提交事务，默认函数退出后提交
        transaction.savepoint_commit(sid)
    except Exception as e:
        # 事务回滚
        transaction.savepoint_rollback(sid)

    return HttpResponse("OK")

```