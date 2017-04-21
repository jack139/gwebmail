drop database if exists webmail;

create database webmail;

use webmail;

create table folder 
(id	     int		not null  auto_increment primary key, 
 uid	     varchar(100)	not null,
 folder_n    varchar(20),
 mails	     numeric(8,0)	default 0,
 unread	     numeric(8,0)	default 0,	
 size	     numeric(10,0)	default 0,
 index	     uid (uid)
) ;

create table mail 
(uid	     varchar(100)	not null,
 folder_n    varchar(20)	not null,
 mail_id     varchar(40)	not null,
 sender	     varchar(100),
 recv	     varchar(100),
 tm	     varchar(30),
 unread	     char(1)		default '1',
 title	     varchar(200),
 size	     numeric(10,0),
 content     mediumblob,
 attach	     varchar(100),
 index	     uid (uid),
 index	     folder_n (folder_n),
 index	     mail_id (mail_id)
) ;

create table user 
(uid	     varchar(100)	not null,
 pwd	     varchar(50)	not null,
 id	     mediumint(9)	not null auto_increment primary key,
 gid	     mediumint(9)	default '500',
 gecos	     char(32),
 home_dir    char(32)		default '/home',
 shell	     char(32)		default 'noshell',
 maildrop    char(128),
 name	     varchar(20),
 addr	     varchar(200),
 ques	     varchar(200),
 ans	     varchar(200),
 index	     uid (uid)
) ;


create table active
(uid	     varchar(100)	not null,
 des	     varchar(40)	not null,
 login_t     numeric(10,0),
 index	     uid (uid),
 index	     des (des)
) ;

create table alias 
(address     varchar(128),
 alias	     varchar(128)
) ;

create table domainalias
(address     varchar(128),
 alias       varchar(128)
) ;

create table domains
(domain	     varchar(128)
) ;

 
